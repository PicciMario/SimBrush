from Tkinter import *
import subprocess, logging, sys, os, sqlite3

class SimUI:
	
	def __init__(self, master):
		
		# ---------- Globals --------------------------------------------------------------------------------
		
		self.path = "../wrapper/images/"
		self.configFile = "simdata.sbr"	
		
		# config file database connection
		self.configConn = None
		self.configCursor = None	
		
		# ---------- User Interface -------------------------------------------------------------------------
		
		self.frame = Frame(master)
		self.frame.pack()
		
		self.button = Button(self.frame, text="QUIT", fg="red", command=self.quitUi)
		self.button.pack(side=LEFT)
		
		self.hi_there = Button(self.frame, text="HELLO", fg="blue", command=self.createReport)
		self.hi_there.pack(side=LEFT)
		
		self.openConfigFile = Button(self.frame, text="Open Config File", fg="blue", command=self.openConfigFile)
		self.openConfigFile.pack(side=LEFT)

		self.closeConfigFileButton = Button(self.frame, text="Close Config File", fg="blue", command=self.closeConfigFile)
		self.closeConfigFileButton.pack(side=LEFT)
		
		# ----------- Logger --------------------------------------------------------------------------------

		# Logger
		self.log = logging.getLogger('SIM Manager')
		self.log.setLevel(logging.DEBUG)
		#create console handler and set level to debug
		ch = logging.StreamHandler()
		ch.setLevel(logging.DEBUG)
		#create formatter
		formatter = logging.Formatter("%(name)s - %(levelname)s - %(message)s")
		#add formatter to ch
		ch.setFormatter(formatter)
		#add ch to logger
		self.log.addHandler(ch)
	
	# open sqlite3 config file $path+$configFile
	def openConfigFile(self):
	
		fullConfigPath = "%s%s"%(self.path, self.configFile)
		self.log.info("Opening config file \"%s\""%fullConfigPath)
		
		# check file existence
		if (os.path.isfile("%s"%fullConfigPath) == 0):
			self.log.critical("Unable to open config file \"%s\": file doesn't exist."%fullConfigPath)
			self.log.debug("Error: %s"%sys.exc_info()[1])
			sys.exit(1)
		
		# try to connect to database file
		try:
			self.configConn = sqlite3.connect("%s"%fullConfigPath)
			self.configCursor = self.configConn.cursor()
		except:
			self.log.critical("Unable to open config file \"%s\": file may be damaged."%fullConfigPath)
			self.log.debug("Error: %s"%sys.exc_info()[1])
			sys.exit(1)	
		
		self.readConfigKey("mille")	
		self.writeConfigKey("newkey", "dioprete")
		self.readConfigKey("newkey")
	
	# close sqlite3 config file	
	def closeConfigFile(self):
		self.log.info("Closing configuration database connection.")
		try:
			self.configConn.commit()
			self.configConn.close()
			self.log.info("Configuration database closed.")
		except:
			self.log.info("Configuration database was already closed.")
	
	# read key from configuration file
	def readConfigKey(self, key):
		
		configDbName = "simdata"
		
		# verify the key has been passed
		if (len(key) == 0):
			self.log.debug("Requested to read a key with no name :-)")
			return None
		
		# try to read key from table "simdata" in database
		try:
			query = "SELECT value FROM %s WHERE id = \"%s\""%(configDbName, key)
			self.configCursor.execute(query)
			result = self.configCursor.fetchone()
			if (result != None):
				self.log.debug("Read key \"%s\" with value \"%s\" from config database."%(key, result[0]))
				return result[0]
		except:
			self.log.warning("Error while trying to read key \"%s\" from config database."%key)
			self.log.debug("Error: %s"%sys.exc_info()[1])
		
		self.log.warning("Requested non existing key \"%s\" from config database."%key)
		return None
		
	def writeConfigKey(self, key, value):
		
		configDbName = "simdata"
		
		# verify the key has been passed
		if (len(key) == 0):
			self.log.debug("Requested to read a key with no name :-)")
			return ""
		
		# look if key already exists
		oldValue = self.readConfigKey(key)
		
		if (oldValue == None):
			query = "INSERT INTO simdata VALUES (\"%s\", \"%s\")"%(key, value)
		else:
			query = "UPDATE simdata SET value=\"%s\" WHERE id=\"%s\""%(value, key)

		try:
			result = self.configCursor.execute(query)
			if (result):
				stringResult = "OK"
			else:
				stringResult = "FAIL"
			self.log.debug("Updated key \"%s\" with value \"%s\". The query ended with result %s"%(key, value, stringResult))
		except:
			self.log.warning("Error while trying to write key \"%s\" with value \"%s\" into config database."%(key, value))
			self.log.debug("Error: %s"%sys.exc_info()[1])
		
					
	
	def say_hi(self):
		print("mille")
	
	def createReport(self):
		self.log.info("Creating report...")
		
		command = [
			"../wrapper/report.py",
			"-f",
			"../wrapper/images/w_test.xml",
			"-v"
		]
		p = subprocess.Popen(command, shell=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		
		while True:
			o = p.stdout.readline()
			if o == '' and p.poll() != None: break
			print o
		
		retval = p.wait()
		print("retval: %i"%retval)
	
	def quitUi(self):
		self.closeConfigFile()
		self.frame.quit()

root = Tk()
app = SimUI(root)
root.mainloop()
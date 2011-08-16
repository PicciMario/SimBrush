#!/usr/bin/env python

# SimBrush SIM Management Interface
# This interface provides all the facilities required to manage a single SIM, wrapping all other utilities:
# carver, wrapper, reporter as well as case data and hash checking.

from Tkinter import *
import subprocess, logging, sys, os, sqlite3

#################################################################################################################

class ConfigDB:
	def __init__(self, path, configFile):
		
		# ---------- Globals --------------------------------------------------------------------------------
		
		self.path = path
		self.configFile = configFile
		
		# config file database connection
		self.configConn = None
		self.configCursor = None

		# ----------- Logger --------------------------------------------------------------------------------

		# Logger
		self.log = logging.getLogger('Config DB Manager')
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
	
		# open configuration sqlite3 file
		# return:
		# 1 - fail
		# 0 - ok 
	
		fullConfigPath = "%s%s"%(self.path, self.configFile)
		self.log.info("Opening config file \"%s\""%fullConfigPath)
		
		# check file existence
		if (os.path.isfile("%s"%fullConfigPath) == 0):
			self.log.critical("Unable to open config file \"%s\": file doesn't exist."%fullConfigPath)
			self.log.debug("Error: %s"%sys.exc_info()[1])
			return 1
		
		# try to connect to database file
		try:
			self.configConn = sqlite3.connect("%s"%fullConfigPath)
			self.configCursor = self.configConn.cursor()
			return 0
		except:
			self.log.critical("Unable to open config file \"%s\": file may be damaged."%fullConfigPath)
			self.log.debug("Error: %s"%sys.exc_info()[1])
			return 1
	
	# close sqlite3 config file	
	def closeConfigFile(self):
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
	
#################################################################################################################

class SimUI:
	
	def __init__(self, master):
		
		# ---------- Globals --------------------------------------------------------------------------------
		
		self.path = "../wrapper/images/"
		
		self.configFile = "simdata.sbr"	
		self.configDB = ConfigDB(self.path, self.configFile)
		
		# executables for SimBrush tools
		self.carveExe = ""
		self.wrapperExe = ""
		self.reportExe = "../wrapper/report.py"
		
		# ---------- User Interface -------------------------------------------------------------------------
		
		self.frame = Frame(master)
		self.frame.pack()
		
		self.button = Button(self.frame, text="QUIT", fg="red", command=self.quitUi)
		self.button.pack(side=LEFT)
		
		self.hi_there = Button(self.frame, text="HELLO", fg="blue", command=self.createReport)
		self.hi_there.pack(side=LEFT)
		
		self.openConfigFile = Button(self.frame, text="Open Config File", fg="blue", command=self.configDB.openConfigFile)
		self.openConfigFile.pack(side=LEFT)

		self.closeConfigFileButton = Button(self.frame, text="Close Config File", fg="blue", command=self.configDB.closeConfigFile)
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
	
		# ----------- Open config DB ------------------------------------------------------------------------
	
		openStatus = self.configDB.openConfigFile()
		if (openStatus != 0):
			self.log.critical("Unable to open configuration file. Quitting.")
			sys.exit(1)			

	
	def createReport(self):
		self.log.info("Creating SimBrush report.")
		
		# report filename (default if not present in config db)
		reportFilename = self.configDB.readConfigKey("report_filename")
		if (reportFilename == None):
			reportFilename = "report.pdf"
			self.configDB.writeConfigKey("report_filename", reportFilename)
		
		# report log filename (default if not present in config db)
		reportLogFilename = self.configDB.readConfigKey("report_log_filename")
		if (reportLogFilename == None):
			reportLogFilename = "reporter_log.txt"
			self.configDB.writeConfigKey("report_log_filename", reportLogFilename)
		
		# wrapped filename
		wrappedFilename = self.configDB.readConfigKey("wrapped_filename")
		if (wrappedFilename == None):
			self.log.error("Wrapped file not listed in config file.")
			return 1
		if (os.path.isfile("%s%s"%(self.path, wrappedFilename)) == 0):
			self.log.error("Trying to create report from unexisting wrapped file %s%s"%(self.path, wrappedFilename))
			return 1
			
		# build investigation data XML file from data in the config DB
		investFilename = "%sinvdata.tmp"%self.path
		investData = []
		
		# invest tags: xml tag, config key
		investTags = [
			['case_number', 'case_number'],
			['case_name', 'case_name'],
			['inv_name', 'inv_name'],
			['sim_number', 'sim_number'],
			['sim_desc', 'sim_desc'],
		]
		
		investFileAvailable = 0
		try:
			investFile = open(investFilename, 'w')
			investFile.write("<xml>\n")	
			for investTag in investTags:
				xmlTag = investTag[0]
				configKey = investTag[1]
				configValue = self.configDB.readConfigKey(configKey)
				if (configValue != None):
					investFile.write("<%s>\n%s\n</%s>\n"%(xmlTag, configValue, xmlTag))
			investFileAvailable = 1
			investFile.write("</xml>\n")
			investFile.close()
		except:
			self.log.error("Unable to write investigation file data")
			self.log.debug("Error: %s"%sys.exc_info()[1])
		
		# build command sequence
		command = [
			self.reportExe,
			"-f",
			"%s%s"%(self.path, wrappedFilename),
			"-o",
			"%s%s"%(self.path, reportFilename),
			"-v"
		]
		
		if (investFileAvailable == 1):
			command.append("-i")
			command.append(investFilename)
		
		# print command sequence
		commandString = ""
		for element in command:
			commandString = "%s %s"%(commandString, element)
		self.log.debug("Executing: %s"%commandString)
		
		# run command
		p = subprocess.Popen(command, shell=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		
		# save stdout to array for logging
		reporterLogData = []
		while True:
			o = p.stdout.readline()
			if o == '' and p.poll() != None: break
			reporterLogData.append(o)
		
		retval = p.wait()
		reporterLogData.append("Return Value: %i"%retval)
		self.log.info("Reporter tool ended with exit status: %s"%retval)
		
		# write reporter log file
		try:
			reportFile = open("%s%s"%(self.path, reportLogFilename), 'w')
			for row in reporterLogData:
				reportFile.write(row)
			reportFile.close()
			self.log.info("Written reporter log to file %s%s"%(self.path, reportLogFilename))
		except:
			self.log.warning("Error while trying to write reporter log file %s%s."%(self.path, reportLogFilename))
			self.log.debug("Error: %s"%sys.exc_info()[1])			
	
		# remove temp investigation xml data file
		if (os.path.isfile(investFilename) != 0):
			try:
				os.remove(investFilename)
			except:
				self.log.warning("Unable to remove temp investigation xml data file %s"%investFilename)
				
	def quitUi(self):
		self.configDB.closeConfigFile()
		self.log.info("Quitting.")
		self.frame.quit()

#################################################################################################################

root = Tk()
app = SimUI(root)
root.mainloop()
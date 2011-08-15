from Tkinter import *
import subprocess

class MainUI:
	
	def __init__(self, master):
		frame = Frame(master)
		frame.pack()
		
		self.button = Button(frame, text="QUIT", fg="red", command=frame.quit)
		self.button.pack(side=LEFT)
		
		self.hi_there = Button(frame, text="HELLO", fg="blue", command=self.createReport)
		self.hi_there.pack(side=LEFT)
		
	def say_hi(self):
		print("mille")
	
	def createReport(self):
		command = "../wrapper/report.py -f ../wrapper/images/w_test.xml -v"
		p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		
		while True:
			o = p.stdout.readline()
			if o == '' and p.poll() != None: break
			print o
		
		retval = p.wait()
		print("retval: %i"%retval)

root = Tk()
app = MainUI(root)
root.mainloop()
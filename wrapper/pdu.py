#!/usr/bin/env python

############################################################################
#    Copyright (C) 2006 by Costin Stroie                                   #
#    cstroie@users.sourceforge.net                                         #
#                                                                          #
#    This program is free software; you can redistribute it and/or modify  #
#    it under the terms of the GNU General Public License as published by  #
#    the Free Software Foundation; either version 2 of the License, or     #
#    (at your option) any later version.                                   #
#                                                                          #
#    This program is distributed in the hope that it will be useful,       #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#    GNU General Public License for more details.                          #
#                                                                          #
#    You should have received a copy of the GNU General Public License     #
#    along with this program; if not, write to the                         #
#    Free Software Foundation, Inc.,                                       #
#    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
############################################################################

# Credits to:
#    Dave Berkeley <dave@rotwang.co.uk> for the pysms project
#    Dominik Pytlewski <d.pytlewski@gazeta.pl> for DoomiN's Phone Manager program

# Import variuos libraries
import sys
from string import *

# Hex to dec conversion array
hex2dec = {'0':0, '1':1, '2':2, '3':3,'4':4, '5':5, '6':6, '7':7,'8':8, '9':9, 'A':10, 'B':11, 'C':12, 'D':13, 'E':14, 'F':15 }

# GSM to ISO8859-1 conversion array
gsm_to_latin = {'0':64, '1':163, '2':36, '3':165,'4':232, '5':233, '6':249, '7':236,'8':242, '9':199,
		'11':216, '12':248,
		'14':197, '15':229, '16':0, '17':95,
		'18':0, '19':0, '20':0, '21':0, '22':0, '23':0, '24':0, '25':0, '26':0, '27':0,
		'28':198, '29':230, '30':223, '31':201,
		'36':164,
		'64':161,
		'91':196, '92':214, '93':209, '94':220, '95':167, '96':191,
		'123':228, '124':246, '125':241, '126':252, '127':224}


def hex2int(n):
	"""
	Convert a hex number to decimal
	"""
	c1 = n[0]
	c2 = n[1]

	c3 = (hex2dec[c1] * 16) + (hex2dec[c2])
	return int("%s" % c3)


def int2hex(n):
	"""
	Convert a decimal number to hexadecimal
	"""
	hex = ""
	q = n
	while q > 0:
		r = q % 16
		if   r == 10: hex = 'A' + hex
		elif r == 11: hex = 'B' + hex
		elif r == 12: hex = 'C' + hex
		elif r == 13: hex = 'D' + hex
		elif r == 14: hex = 'E' + hex
		elif r == 15: hex = 'F' + hex
		else:
			hex = str(r) + hex
		q = int(q/16)

	if len(hex) % 2 == 1: hex = '0' + hex
	return hex


def byteSwap(byte):
	"""
	Swap the first and second digit position inside a hex byte
	"""
	return "%c%c" % (byte[1], byte[0])


def parseTimeStamp(time):
	"""
	Convert the time from PDU format to some common format
	"""

	y = byteSwap(time[0:2])
	m = byteSwap(time[2:4])
	d = byteSwap(time[4:6])

	hour = byteSwap(time[6:8])
	min = byteSwap(time[8:10])
	sec = byteSwap(time[10:12])

	if int(y) < 70:
		y = "20" + y

	return "%s.%s.%s %s:%s" % (y, m, d, hour, min)


def decodeText7Bit(src):
	"""
	Decode the 7-bits coded text to one byte per character
	"""

	bits = ''

	i = 0
	l = len(src) - 1

	# First, get the bit stream, concatenating all binary represented chars
	while i < l:
		bits += char2bits(src[i:i+2])
		i += 2

	# Now decode those pseudo-8bit octets
	char_nr = 0
	i = 1

	tmp_out = ''
	acumul = ''
	decoded = ''
	while char_nr <= len(bits):
		byte = bits[char_nr + i:char_nr + 8]
		tmp_out += byte + "+" + acumul + " "
		byte += acumul
		c = chr(bits2int(byte))

		decoded += c

		acumul = bits[char_nr:char_nr + i]

		i += 1
		char_nr += 8

		if i==8:
			i = 1
			char_nr
			decoded += chr(bits2int(acumul))
			acumul=''
			tmp_out += "\n"

	return gsm2latin(decoded)


def decodeText8Bit(src):
	"""
	Decode the 8-bits coded text to one byte per character
	"""
	chars = ''
	i = 0
	while i < len(src):
		chars += chr(src[i:i + 2])
		i += 2
	return chars


def decodeText16Bit(src):
	"""
	Decode the 16-bits coded text to one byte per character
	"""
	chars = u''
	i = 0
	while i < len(src):
		h1 = src[i:i + 2]
		h2 = src[i + 2:i + 4]
		c1 = hex2int(h1)
		c2 = hex2int(h2)

		unicodeIntChar = (256 * c1) + c2
		unicodeChar = unichr(unicodeIntChar) #chr
		
		chars += unicodeChar
		i += 4
	return chars


def encodeText7Bit(src):
	"""
	Encode ASCII text to 7-bit encoding
	"""
	result = []
	count = 0
	last = 0
	for c in src:
		this = ord(c) << (8 - count)
		if count:
			result.append('%02X' % ((last >> 8) | (this & 0xFF)))
		count = (count + 1) % 8
		last = this
	result.append('%02x' % (last >> 8))
	return ''.join(result)


def char2bits(char):
	"""
	Convert a character to binary.
	"""

	inputChar = hex2int(char)
	mask = 1
	output = ''
	bitNo = 1

	while bitNo <= 8:
		if inputChar & mask > 0:
			output = '1' + output
		else:
			output = '0' + output
		mask = mask<<1
		bitNo += 1

	return output


def bits2int(bits):
	"""
	Convert a binary string to a decimal integer
	"""

	mask = 1
	i = 0
	end = len(bits) - 1

	result = 0
	while i <= end:
		if bits[end - i] == "1":
			result += mask
		mask = mask << 1
		i += 1

	return result


def gsm2latin(gsm):
	"""
	Convert a GSM encoded string to latin1 (where available).
	TODO: implement the extension table introduced by char 27.
	"""

	i = 0
	latin = ''
	while i < len(gsm) - 1:
		if str(ord(gsm[i])) in gsm_to_latin:
			latin += chr(gsm_to_latin[str(ord(gsm[i]))])
		else:
			latin += gsm[i]
		i += 1

	return latin


class PDU:
	"""
	The PDU class implements basic encoding and decoding functions related
	to the GSM PDU format. It also contains high level functions for
	phonebook and sms processing.
	"""

	def __init__(self):
		"""
		Class initialisation routines
		"""


	def decodeSMS(self, sms):
		"""
		Return the SMS and extra SMS data decoded from the PDU format
		"""
		self.SMS = {}

		smsc_len = hex2int(sms[0:2])
		type_of_address = hex2int(sms[2:4])

		# Read the SMS center number
		smsc_number = ''
		i = 4
		while i/2 <= smsc_len:
			smsc_number += sms[i+1]
			if i/2 != smsc_len:
				smsc_number += sms[i]
			i+=2
		self.SMS['smsc'] = smsc_number

		# Decode the sms sender information
		pdu_flags = hex2int(sms[i:i+2])

		# Message type indicator. Bits no 1 and 0 are both set to 0 to indicate that this PDU is an SMS-DELIVER.
		if pdu_flags & 1 == 0 and pdu_flags & 2 == 0:
			self.SMS['fMTI'] = True
		else:
			self.SMS['fMTI'] = False

		# More messages to send. This bit is set to 0 if there are more messages to send.
		if pdu_flags & 4 != 0:
			self.SMS['fMMS'] = True
		else:
			self.SMS['fMMS'] = False

		# Status report indication. This bit is set to 1 if a status report is going to be returned to the SME.
		if pdu_flags & 32 != 0:
			self.SMS['fSRI'] = True
		else:
			self.SMS['fSRI'] = False

		# User data header indicator. This bit is set to 1 if the User Data field starts with a header.
		if pdu_flags & 64 != 0:
			self.SMS['fUDHI'] = True
		else:
			self.SMS['fUDHI'] = False

		# Reply path. Parameter indicating that reply path exists.
		if pdu_flags & 128 != 0:
			self.SMS['fRP'] = True
		else:
			self.SMS['fRP'] = False

		# Decode the sender number
		i += 2
		sender_len = hex2int(sms[i:i+2])
		i += 2
		sender_type = sms[i:i+2]
		i += 2
		if sender_len % 2 == 1:
			# with a trailing F
			sender_number = sms[i:i + sender_len + 1]
			i += sender_len + 1
		else:
			sender_number = sms[i:i + sender_len]
			i += sender_len

		tmp_sender = ''
		s = 0
		e = len(sender_number) - 1

		while s < e:
			tmp_sender += sender_number[s+1]
			tmp_sender += sender_number[s]
			s+=2

		if sender_len % 2 == 1:
			# need to cut of 'F'
			tmp_sender = tmp_sender[0:-1]

		self.SMS['sender'] = tmp_sender

		# Loading DATA CODING SCHEME and FLAGS
		protocol_id = sms[i:i+2]
		i += 2
		dcs_mode = sms[i:i+2]
		i += 2

		dcs_bits = hex2int(dcs_mode)
		if dcs_bits & 128 == 1 and dcs_bits & 64 == 1 and dcs_bits & 32 == 1 and dcs_bits & 16 == 1:
			# Data coding/message class
			special_coding = True
		else:
			special_coding = False

		if (dcs_bits & 8 == 0 and dcs_bits & 4 == 0) or special_coding == True:
			self.SMS['coding'] = 7
		elif dcs_bits & 8 > 0 and dcs_bits & 4 == 0:
			self.SMS['coding'] = 16
		elif dcs_bits & 8 == 0 and dcs_bits & 4 > 0:
			self.SMS['coding'] = 8
		else:
			self.SMS['coding'] = 0

		# Get the sending timestamp
		self.SMS['time'] = parseTimeStamp(sms[i:i+14])

		# Finally, get the message
		i += 14
		self.SMS['length'] = hex2int(sms[i:i+2])

		if self.SMS['length'] > 0:
			i += 2
			message_enc = sms[i:i + (self.SMS['length'] * 2) - 1]

		if self.SMS['coding'] == 7:
			self.SMS['message'] = decodeText7Bit(message_enc)
		elif self.SMS['coding'] == 8:
			self.SMS['message'] = decodeText8Bit(message_enc)
		else:
			self.SMS['message'] = decodeText16Bit(message_enc)


	def encodeSMS(self, dest_number, text):
		"""
		Return the SMS encoded to PDU format, based on a template
		and on supplied data.
		TODO: Accepted phone number is in International format only.
		"""

		# Start from a template
		result = "001100"

		# Add the length of the phone number and the number format
		result += int2hex(len(dest_number)) + '91'

		# Add the phone number
		tmp_number = dest_number
		if len(tmp_number) % 2 == 1: tmp_number += 'F'
		i = 0
		while i < len(tmp_number):
			result = result + byteSwap(tmp_number[i:i+2])
			i += 2

		# Add the protocol id and DCS
		result += '0000'

		# Add the validity period (4 days)
		result += 'AA'

		# Added the message length and the message
		result += int2hex(len(text))
		result += encodeText7Bit(text)

		return result



if __name__=="__main__":
	x = PDU()
	
	x.decodeSMS("0791947101670000040C919461805619410000909070903093804CCA300B14169BC368391D949ED3416171590E9A8FD16F37A8DE06C56A6816683E4687CDE6391D44AE83C8E139485F4ED3D9E931FA073ACBEB1E16688C96A7E7F4371C0DFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")
	print x.SMS
	print unicode(x.SMS['message'], 'latin-1')

# vim: set nu nowrap ft=python syn=python:

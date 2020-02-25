import re

instructions = {
'HLT_':0,
'MOV_04':1,
'MOV_01':2,
'MOV_00':3,
'MOV_14':4,
'MOV_03':5,
'MOV_10':6,
'MOV_24':7,
'MOV_02':8,
'MOV_20':9,
'MOV_30':10,
'MOV_34':11,

'JMP_0':12,
'JC_0':13,
'JZ_0':14,
'JBE_0':15,
'JNC_0':16,
'JNZ_0':17,
'JNBE_0':18,

'JMP_4':19,
'JC_4':20,
'JZ_4':21,
'JBE_4':22,
'JNC_4':23,
'JNZ_4':24,
'JNBE_4':25,

'PUSH_0':26,
'PUSH_4':27,

'POP_0':28,

'ADD_00':29,
'ADD_04':30,

'SUB_00':31,
'SUB_04':32,

'MUL_4':33,
'MUL_0':34,

'DIV_4':35,
'DIV_0':36,

'INC_0':37,

'DEC_0':38,

'CMP_00':39,
'CMP_03':40,
'CMP_02':41,
'CMP_04':42,

'SHL_4':43,
'SHL_0':44,

'SHR_4':45,
'SHR_0':46,

'AND_04':47,
'AND_00':48,

'OR_04':49,
'OR_00':50,

'XOR_04':51,
'XOR_00':52,

'NOT_0':53,

'CALL_0':54,
'CALL_4':55,

'RET_':56,

'LEA_02':57,

'DR_':58,
'DF_':59

}

'''
opcode id format:
inst + '_' + defcode.

defcode:

0: reg
1: reg addr
2: reg +/- offset addr
3: const addr
4: const

'''

registers = {
'A':0,
'B':1,
'C':2,
'D':3,
'BP':4,
'SP':5
}

labels = {}

def isnumeric_array(data):
	for c in data:
		if not c.isnumeric():
			return 0
	return 1

def check_label(c,raw=False):
	if '%' in c:
		regex = re.compile(r'%((?=[a-zA-z_\.])[a-zA-Z0-9_\.]*)')
		if not raw:
			return c.replace(regex.search(c).group(0), str(labels[regex.search(c).group(1)]))
		return str(labels[regex.search(c).group(1)])
	return c

def getType(data):
	if (re.match(r'^\w{2,3}(?!:)',data)):
		return 0
	if (re.search(r'^\s*\w+:', data) is not None):
		return 1
	return -1


def getArgMode(inst, raw=False): 
	am = str('')
	syntaxErr = True

	for c in inst:

		if (':' in c):
			am = "skip"
			break

		if '%' in c:
			c = check_label(c)

		if c.upper() in registers: # register.
			am += '0'
			syntaxErr = False
		elif (re.search(r'\[[a-z]{1,2}\]',str(c)) is not None): # register addr.
			am += '1'
			syntaxErr = False
		elif (re.search(r'\[\w{1,2}(\+|\-)(\d{1,2}|0x[a-f0-9]{1,2})\]', str(c)) is not None): # register +/- offset.
			am += '2'
			syntaxErr = False
		elif (re.search(r'\[(0x)?[a-f0-9]{1,2}\]|\[\d{1,3}\]', str(c)) is not None): # const-addr.
			am += '3'
			syntaxErr = False
		elif (re.search(r'0x[a-f0-9]{1,2}|\d{1,3}', str(c)) is not None): # const.
			am += '4'
			syntaxErr = False

	if (raw == False):
		return (inst[0] + "_" + am)
	else:
		return am

def add_label(name, data):
	labels[name] = data;

def translate(data):
	mode = getArgMode(data)
	raw_mode = getArgMode(data,True)
	bytelist = []

	if isnumeric_array(data) == 1:
		for n in data:
			bytelist.append(int(n))
		return bytelist

	for i, c in enumerate(data):

		if ';' in c:
			break

		if (i == 0) and (raw_mode != "skip"):

			if instructions.get(mode.upper()):
				bytelist.append(instructions[mode.upper()])
			else:
				if mode.upper() == 'HLT_':
					bytelist.append(0)
					continue
				errmsg = "ERR brasm > '"+', '.join(data)+"', opcode: "+mode+" can't be found."
				print(errmsg)
				exit()

			continue

		if raw_mode[i-1] == '0':
			bytelist.append(registers[c.upper()])
		
		elif raw_mode[i-1] == '1':
			bytelist.append(registers[re.sub(r'[\[\]]','',c.upper())])
		
		elif raw_mode[i-1] == '2':
			
			data = re.match(r'\[(\w{1,2})(\+|\-)(\d{1,2}|0x[a-f0-9]{1,2})\]',c.upper())
			r = registers[data.group(1)]
			s = data.group(2)
			c = int(data.group(3), 0)
			build_flag = int()
			
			if s == '+':
				s = 0
			else:
				s = 1

			build_flag = build_flag | r
			build_flag = build_flag | (s << 7)
			build_flag = build_flag | (c << 3)

			bytelist.append(build_flag)

		elif (raw_mode[i-1] == '3') or (raw_mode[i-1] == '4'):
			
			c = check_label(c,True)

			bytelist.append(int(c,0)) 


	return bytelist

def assemble(data=None):

	if (data == None):
		file_name = input("File name: ")
		file_out = re.sub(r'(?!\w+)\.\w{1,3}', '.out', file_name)

	with open(file_name,"r") as file:
		lines = file.readlines()
		lines = [x.strip() for x in lines]
		lines = [x.replace(',','') for x in lines]

		inst = []

		print("brasm > grouping instruction data.")

		replace_array = []

		for i, c in enumerate(lines):
			if ".STR" in c.upper():
				strings = re.findall(r'(?<=\").+(?=\")', c)
				for s in strings:
					replace_array.append("DB")
					for c2 in s:
						replace_array.append(str(ord(c2)))
					lines[i] = " ".join(replace_array)
					replace_array = []

		print(lines)

		for i, c in enumerate(lines):
			inst.append(re.findall(r'\S+', lines[i]))
			inst = [x for x in inst if x != []]

		for i, c in enumerate(inst):
			if "DZ" in c[0].upper():
				for i2 in range(int(inst[i][1],0)):
					replace_array.append('0')
				inst[i] = replace_array
				replace_array = []
				continue 
			if "DB" in c[0].upper():
				for i2 in range(1,len(inst[i])):
					replace_array.append(inst[i][i2])
				inst[i] = replace_array
				replace_array = []
				continue

		for x in inst:
			print(x)

		ip = 0
		print("brasm > locating labels.")
		for i, c in enumerate(inst):

			label = re.search(r'((?=[a-zA-z_\.])[a-zA-Z0-9_\.]*):',c[0])

			hasComment = False

			for i, c2 in enumerate(c):
				if ';' in c2:
					hasComment = True
				if hasComment:
					c[i].replace(c2, '')


			if label == None:
				ip += len(c)
				continue

			if not label.group(1) in labels:
				labels[label.group(1)] = ip
				print("brasm > added label {0} at location {1}.".format(label.group(1),str(ip)))


		if labels == {}:
			print("brasm > no labels found.")
		else:
			print(labels)

		ip = 0
		bytelist2 = [] #binary version.

		for i, c in enumerate(inst):
			if not getType(c[0]) == 1:
				if translate(c) != []:
					for x in translate(c):
						bytelist2.append(x)
				ip += len(c)

		print('brasm > Memory used: ',str(len(bytelist2))," bytes.")
		print('brasm > Memory left: ',str(255-len(bytelist2))," bytes.")

		with open(file_out,"wb") as file:
			file.write(bytes(bytelist2))
			file.close()

def main():
	assemble()

main()
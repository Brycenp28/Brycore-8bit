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
'CMP_01':43,

'SHL_4':44,
'SHL_0':45,

'SHR_4':46,
'SHR_0':47,

'AND_04':48,
'AND_00':49,

'OR_04':50,
'OR_00':51,

'XOR_04':52,
'XOR_00':53,

'NOT_0':54,

'CALL_0':55,
'CALL_4':56,

'RET_':57,

'LEA_02':58,

'INT_4': 59,
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

labels = {} # list for all the labels with adresses.

def isnumeric_array(data):
	for c in data:
		if not c.isnumeric():
			return 0
	return 1

def get_local_label(label_name,ip):

	n = []
	v = []
	location = 0

	for names, values in labels.items():
		if '.' in names:
			continue
		n.append(names)
		v.append(values)

	print("ip: ", ip)

	for i, value in enumerate(v):
		print("index: {0}, value: {1}".format(i,value))

		if ip >= value:
			location = i

	name = n[location] + label_name

	print("Name: {0}, location: {1}.".format(name,labels[name]))

	return str(labels[name])


def check_label(c,ip,raw=False):
	if '%' in c:
		regex = re.compile(r'%((?=[a-zA-z_\.])[a-zA-Z0-9_\.]*)')
		label = regex.search(c).group(1) #str(labels[regex.search(c).group(1)])

		if label[0] == '.':
			label = get_local_label(label,ip)
		else:
			label = str(labels[regex.search(c).group(1)])

		if not raw:
			return c.replace(regex.search(c).group(0), label)

		return label

def getType(data):
	if (re.search(r'((?=[a-zA-z_\.])[a-zA-Z0-9_\.]*):', data) is not None):
		return 1
	else:
		return 0


def getArgMode(inst, ip, raw=False): 
	am = str('')
	syntaxErr = True

	for c in inst:

		if (':' in c):
			am = "skip"
			break

		if '%' in c:
			c = check_label(c,ip)


		if c.upper() in registers: # register.
			am += '0'
			syntaxErr = False
		elif (re.search(r'\[[a-z]{1,2}\]',str(c)) is not None): # register addr.
			am += '1'
			syntaxErr = False
		elif (re.search(r'\[\w{1,2}(\+|\-)(\d{1,2})\]', str(c)) is not None): # register +/- offset.
			am += '2'
			syntaxErr = False
		elif (re.search(r'\[\d{1,3}\]', str(c)) is not None): # const-addr.
			am += '3'
			syntaxErr = False
		elif (re.search(r'\d{1,3}', str(c)) is not None): # const.
			am += '4'
			syntaxErr = False

	if (raw == False):
		return (inst[0] + "_" + am)
	else:
		return am

def add_label(name, data):
	labels[name] = data;

def translate(index, data, ip):
	mode = getArgMode(data,ip)
	raw_mode = getArgMode(data, ip, True)
	bytelist = []
	print(mode)

	if isnumeric_array(data) == 1:
		for n in data:
			bytelist.append(int(n))
		return bytelist

	for i, c in enumerate(data):

		if ';' in c:
			break

		if '%' in c:
			c = check_label(c,ip)

		if (i == 0) and (raw_mode != "skip"):

			if instructions.get(mode.upper()):
				bytelist.append(instructions[mode.upper()])
			else:
				if mode.upper() == 'HLT_':
					bytelist.append(0)
					continue
				errmsg = "ERR brasm > [Instruction: {0}]: '{1}', Invalid syntax.".format(index,' '.join(data))
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

		elif (raw_mode[i-1] == '3'):
			c = c.replace('[', '')
			c = c.replace(']', '')
			bytelist.append(int(c,0))
		elif (raw_mode[i-1] == '4'):
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

		ip = 0
		print("brasm > locating labels.")

		last_label = str()

		for i, c in enumerate(inst):

			label = re.search(r'((?=[a-zA-z_\.])[a-zA-Z0-9_\.]*):',c[0])


			for i, c2 in enumerate(c): # strips comments.
				if ';' in c2:
					for i2 in range(len(c) - i):
						c.pop(i)
					break

			if (label == None):
				ip += len(c)
				continue
			else:
				if '.' in label.group(1):
					labels[label.group(1).replace('.',last_label + '.')] = ip
					print("brasm > added local label for {0} at location {1}.".format(last_label,ip))
					continue
				else:
					last_label = label.group(1)

			if (not label.group(1) in labels) or (not last_label+label.group(1) in labels):
				labels[label.group(1)] = ip
				print("brasm > added label {0} at location {1}.".format(label.group(1),str(ip)))
			else:
				print("ERR brasm > Redeclaration of label {0}.".format(label.group(1)))

		if labels == {}:
			print("brasm > no labels found.")
		else:
			print(labels)

		ip = 0
		bytelist2 = [] #binary version.
		for x in labels:
			last_label = x
			break # to get the first label to start the program

		for i, c in enumerate(inst):
			if c == []:
				continue
			if getType(c[0]) == 0:

				t = translate(i,c,ip)

				if t != []:
					for x in t:
						bytelist2.append(x)
				ip += len(c)

		print('brasm > Memory used: ',str(len(bytelist2))," bytes.")
		print('brasm > Memory left: ',str(255-len(bytelist2))," bytes.")

		with open(file_out,"wb") as file:
			file.write(bytes(bytelist2))
			file.close()

def main():
	assemble()

if __name__ == "__main__":
	main()
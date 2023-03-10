num1 = 0
num2 = 1

for i in range(46):
	print(num2)
	tmp = num2
	num2 = num1 + num2
	num1 = tmp

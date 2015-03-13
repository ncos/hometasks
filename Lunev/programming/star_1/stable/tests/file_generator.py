

f_source = open('../talker/workfile', 'w')
f_gold = open('../listener/workfile', 'w')


for i in range(100000):
	f_source.write('0123456789abcdef')
	f_gold.write('0123456789abcdef')

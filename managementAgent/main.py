from pexpect import spawn
import time

def bring_up_new_node(username,ip, pw, commands):
	child = spawn('ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no '+ username +'@'+ip)
	child.expect(username +'@'+ip+"'s password:")
	child.sendline(pw)
	for cm in commands: child.sendline(cm)
	time.sleep(2)
	#for line in child: print(line)
	child.sendline('exit')
	child.sendline('^C')

if __name__=='__main__':
	#bring_up_new_node('magic', '192.168.205.42', 'magic123', ['echo "test from inner" > /home/magic/tt.txt'])
	#bring_up_new_node('magic', '192.168.205.42', 'magic123', ['source /home/magic/mehran-admin-nova', 'nova start new_elasticity_HTTP_4'])
	print('dummy command to bring node4 up')

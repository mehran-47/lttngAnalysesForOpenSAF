from pexpect import spawn
from sys import argv
from threading import Thread

def execute_commands_at(user, ip, pw, commands):
	print('ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no '+user+'@'+ip)
	child_shell = spawn('ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no '+user+'@'+ip, timeout=None)
	child_shell.expect(user+"@"+ip+"'s password:")
	child_shell.sendline(pw)
	for command in commands:
		child_shell.sendline(command)
	for line in child_shell:
		print(line)

def scale_out_vm():
	print('Spawning new VMs...')
	commands = ['source mehran-admin-nova',\
	'cd anti_affinity_demo/AntiAffinityClusterSpawner/',\
	'python vm_spawner.py 2',\
	'exit']
	execute_commands_at('magic', '192.168.205.42', 'magic123', commands)

def increase_assignment_for(SI):
	print('Scaling out')
	commands=['cd /home/node1/Downloads/lttngAnalysesForOpenSAF/',\
	'python -m EE.main '+SI+' 1 1',\
	'exit']
	execute_commands_at('node1', '192.168.10.8', 'magic123', commands)


if __name__ == '__main__':
	if argv[1:]:
		scale_out_vm()
		increase_assignment_for(argv[1])
			

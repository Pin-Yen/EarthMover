import subprocess as sp

result = sp.run(['ls', '-l'], stdout = sp.PIPE)
print(result.stdout)
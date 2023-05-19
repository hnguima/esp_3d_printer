import requests
import hashlib
import argparse


parser = argparse.ArgumentParser(
                    prog = 'ESP - HTTP firmware updater',
                    description = 'Script to update firmware on remote devices via HTTP. Set the device IP and firmware binary path to do the update.')


parser.add_argument( '-b','--binary', metavar='', help='path to the binary to be uploaded')
parser.add_argument('-i', '--ip', metavar='', help= 'IP address of the destination device')

args = parser.parse_args()

fileName = args.binary
deviceIP = args.ip

hash = hashlib.sha256()

with open(fileName, mode='rb') as file: # b is important -> binary

  fileContent = file.read()
  hash.update(fileContent)
  print(hash.hexdigest());
  
  x = requests.put(f'http://{deviceIP}/update_firmware', data=fileContent,
                   headers={'Hash':hash.hexdigest()},
                   timeout=20)

  # print(x.text)
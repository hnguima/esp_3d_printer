import os , shutil
from pathlib import Path

if os.path.exists(f"{os.getcwd()}/generated"):
  shutil.rmtree(f"{os.getcwd()}/generated")

os.mkdir(f"{os.getcwd()}/generated")

if os.path.exists(f"{os.getcwd()}/proto"):
  shutil.rmtree(f"{os.getcwd()}/proto")

os.mkdir(f"{os.getcwd()}/proto")

if os.path.exists(f"{os.getcwd()}/page"):
  shutil.rmtree(f"{os.getcwd()}/page")

os.mkdir(f"{os.getcwd()}/page")

for root, subdirs, files in os.walk(f'{os.getcwd()}/../../'):

  if "proto" in subdirs and "esp_settings" not in str(Path(root).resolve()):

    for path in os.listdir(f'{Path(root).resolve()}/proto'):
      shutil.copyfile(f'{Path(root).resolve()}/proto/{path}', f'{os.getcwd()}/proto/{path}')


for path in os.listdir(f'{os.getcwd()}/proto'):

  if os.path.isfile(os.path.join(f'{os.getcwd()}/proto', path)) and ".proto" in path:
    os.system(f'python {os.getcwd()}/nanopb/generator/nanopb_generator.py -D {os.getcwd()}/generated --cpp-descriptors -I {os.getcwd()}/proto --strip-path {path}')


os.system(f'pbjs -t static-module -w es6 -p {os.getcwd()}/proto  {os.getcwd()}/proto/main.proto  > {os.getcwd()}/page/proto.pb.js')
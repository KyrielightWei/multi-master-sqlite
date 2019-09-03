# run in grpc env dir

export env_dir=$(dirname $(readlink -f "$0"))

echo "export ENV_PATH=$env_dir" >> ~/.bashrc
echo "source $env_dir/multi_env.sh" >> ~/.bashrc

exec /bin/bash

echo "$ENV_PATH >> ~/.bashrc"

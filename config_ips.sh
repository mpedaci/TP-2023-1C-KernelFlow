#!/bin/bash

validate_ip() {
  # Regular expression pattern for IP address validation
  local ip_pattern='^([0-9]{1,3}\.){3}[0-9]{1,3}$'

  if [[ $1 =~ $ip_pattern ]]; then
    return 0  # Valid IP address
  else
    return 1  # Invalid IP address
  fi
}

# Take the replace string
read -p "Enter the kernel IP: " ipKernel
read -p "Enter the memory IP: " ipMemoria
read -p "Enter the fs IP: " ipFS
read -p "Enter the cpu IP: " ipCPU

if [[ $ipKernel == "" || $ipMemoria == "" || $ipFS == "" || $ipCPU == "" ]]; then
  echo "Error: Empty IP"
  exit 0
fi

if validate_ip "$ipKernel" && validate_ip "$ipMemoria" && validate_ip "$ipFS" && validate_ip "$ipCPU"; then
  echo "All entered IP addresses are valid updating configs"
else
  echo "Error: One or more IP addresses are invalid"
  exit 0
fi

# Regexs
sKer="IP_KERNEL=[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}"
sMem="IP_MEMORIA=[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}"
sFs="IP_FILESYSTEM=[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}"
sCpu="IP_CPU=[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}"

# Kernel
kc0="./kernel/config/kernel.config"
kc1="./tests/Configs/kernel/kernel.t1.config"
kc2="./tests/Configs/kernel/kernel.t2.config"
kc3="./tests/Configs/kernel/kernel.t3.config"
kc4="./tests/Configs/kernel/kernel.t4.config"
# Search and replace Memoria
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $kc0
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $kc1
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $kc2
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $kc3
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $kc4
# Search and replace CPU
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $kc0
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $kc1
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $kc2
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $kc3
sed -E -i "s/$sCpu/IP_CPU=$ipCPU/" $kc4
# Search and replace FS
sed -E -i "s/$sFs/IP_FILESYSTEM=$ipFS/" $kc0
sed -E -i "s/$sFs/IP_FILESYSTEM=$ipFS/" $kc1
sed -E -i "s/$sFs/IP_FILESYSTEM=$ipFS/" $kc2
sed -E -i "s/$sFs/IP_FILESYSTEM=$ipFS/" $kc3
sed -E -i "s/$sFs/IP_FILESYSTEM=$ipFS/" $kc4

# Consola
cc0="./consola/config/console.config"
# Search and replace Kernel
sed -E -i "s/$sKer/IP_KERNEL=$ipKernel/" $cc0

# FS
fsc0="./filesystem/config/filesystem.config"
# Search and replace Memoria
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $fsc0

# CPU
cpc0="./cpu/config/cpu.config"
cpc1="./tests/Configs/cpu/cpu.t1.config"
cpc2="./tests/Configs/cpu/cpu.t2.config"
# Search and replace Memoria
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $cpc0
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $cpc1
sed -E -i "s/$sMem/IP_MEMORIA=$ipMemoria/" $cpc2

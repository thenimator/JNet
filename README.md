# JNet
JNet I guess
## Building
1. clone repo
2. open terminal
3. install boost (linux)
`sudo apt-get install libboost-all-dev`

4. execute 
`setup.sh`
5. forward ports if you want to build the server
6. execute either
`building/client.sh`
`building/server.sh`
to execute and build

OR

4. execute command 
`cmake -B build`
5. forward ports if you want to build the server
6. execute command to build executeable
`cmake --build build`
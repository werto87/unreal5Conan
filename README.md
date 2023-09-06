# unreal5Conan

build notes:  
this project is dependent on adamrehn docker image to create boilerplate to allow the usage of conan with Unreal Engine
1.  Install ue4-docker from pip
2.  limit nofile in docker container  
  2.1. Create the file /etc/docker/daemon.json  
  2.2. With the content {
    	"default-ulimits": {
    		"nofile": {
    			"Name": "nofile",
    			"Hard": 1024,
    			"Soft": 1024
    		},
    		"nproc": {
    			"Name": "nproc",
    			"Soft": 65536,
    			"Hard": 65536
    		}
    	}
    }
3. Create the docker image "ue4-docker build  --ue-version 5.2.1-1 --target full  -v &> output.log" (replace 5.2.1-1 with your desired version)
4. if it fails, check this issue "https://github.com/adamrehn/conan-ue4cli/issues/20#issue-1465100084"



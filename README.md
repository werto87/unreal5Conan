# unreal5Conan

build notes:  
this project is dependend on adamrehn docker image to create boilerplate to alow the usage of conan with unreal engine
1.  instal ue4-docker from pip
2.  limit nofile in docker container  
  2.1. create the file /etc/docker/daemon.json  
  2.2. with the content {
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
3. create the docker image "ue4-docker build  --ue-version 5.2.1-1 --target full  -v &> output.log"
4. if it fails check this issue "https://github.com/adamrehn/conan-ue4cli/issues/20#issue-1465100084"

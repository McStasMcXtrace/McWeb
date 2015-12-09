angular.module("myApp").factory("Hub", function(){
    var Hub = function(){
    };

    Hub.prototype = {
        setData: function(json){
//            console.log(json);
            
            this.hostName    = json['Host'];
            this.machineType = json['MachineType'];
            this.cpus        = json['CPUs'];
            this.os          = json['OS'];           
        },
        toJSON: function(){
            return({
                Host: this.host,
                MachineType: this.machineType,
                CPUs: this.cpus,
                OS: this.os
            });
        }
    }

    return Hub;
}).service("HubSet", function(Hub){
    this.elements = [];
    
    this.create = function(response){
        var newS = new Hub();

        newS.setData(response);
        this.elements.push(newS);
//        console.log(this.elements);

        this.currHub = this;
    }

    this.currHub = null;
});

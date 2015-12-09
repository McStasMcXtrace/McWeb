angular.module("myApp").factory("Job", function() {
    var Job = function(){
        
    };

    Job.prototype = {
        statistics: [],
        setData: function(id, json){
//            console.log(json);
            this.id         = id;
            this.uri        = decodeURIComponent(json['uri']);
            this.resource   = decodeURIComponent(json['resource']);
            this.status     = json['status'];
        },
        toJSON: function(){
            return({
                id          : this.id,
                uri         : this.uri,
                resource    : this.resource,
                status      : this.status
            });
        }
    }

    return Job;
}).factory("JobSet", function(Job) {
    
    var JobSet = function() {
        
    };

    var addJobs = function(json, jobSet, checkExistence) {
        jobSet.failedJobs = json.numJobsFailed;

        for (job in json.jobs) {
            var newJobId = job.replace(/job/, '');
            if (checkExistence) {              
                var jobAlreadyExists = jobSet.maximumJobId >= newJobId;
                if (jobAlreadyExists) {
                    // 
                    var checkIndex = jobSet.maximumJobId - newJobId;
                    //console.log(job, newJobId, checkIndex, jobSet.elements[checkIndex]);
                    // Assumes job index is the same as id.
                    jobSet.elements[checkIndex].setData(job, json.jobs[job]);
                    continue;
                }
            }
            
            var newJob = new Job();
            newJob.setData(job, json.jobs[job]);
            jobSet.elements.unshift(newJob);
            
            jobSet.maximumJobId = Math.max(newJobId, jobSet.maximumJobId);
        }
        
        jobSet.isLoaded = true;
    };
    
    JobSet.prototype = {
        elements: [],
        maximumJobId: 0,
        isLoaded: false,
        failedJobs: 0,
        currentJob: null,
        jobChangedListeners: [],
        
        addAllJobs: function(json) {
            addJobs(json, this, false);
        },
        
        updateJobs: function(json) {
            addJobs(json, this, true);
        },
        
        hasCurrentJob: function() {  
            return this.currentJob != null; 
        },
        
        addJobChangedListener: function(listener) {
            var contains = this.jobChangedListeners.indexOf(listener) >= 0;
            if (!contains) {
                this.jobChangedListeners.push(listener);
            }   
        },
        
        removeJobChangedListener: function(listener) {
            var containsIndex = this.jobChangedListeners.indexOf(listener);
            if (containsIndex >= 0) {
                this.jobChangedListeners.splice(containsIndex, 1);
            }          
        },
        
        setCurrentJob: function(newJob) {
            var oldJob = this.currentJob;
            this.currentJob = newJob;
            
            for (listener in this.jobChangedListeners) {
                this.jobChangedListeners[listener](oldJob, newJob);
            }   
        }
    };
    
    return JobSet;
});

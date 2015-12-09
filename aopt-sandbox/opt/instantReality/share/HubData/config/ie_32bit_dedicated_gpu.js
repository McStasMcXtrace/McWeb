{
    "setups" :
        [
            {
                "name" : "Client rendering (WebGL 1) 32bit dedicated GPU IE",
                "tag" : "cl_webgl1",
                "pipelineSetup" : {},
                "settings" :
                {
                    "renderBudgetMs" : 10,
                    "renderBudgetMsDuringNavigation" : 5,
                    "drawJobCreationBudgetMs" : 10,
                    "drawJobCreationBudgetMsDuringNavigation" : 5,
                    "traverserBudgetMs" : 10,
                    "frustumCulling" : true,
                    "smallFeatureCulling" : true,
                    "smallFeatureTreshold" : 0.01,
                    "smallFeaturePixelThreshold" : 32,
                    "resizeCanvasToVisiblePortion" : true,
                    "maxUploadsPerFrame" : 100,
                    "maxMsUploadsPerFrame" : 10,
                    "maxGPUMemory" : 800,
                    "maxSRCCPUMemory" :  256
                }
            },
            {
                "name" : "Remote Rendering (Full)",
                "tag" : "rm_full",
                "pipelineSetup" : {},
                "settings" :
                {
                    "frustumCulling" : true,
                    "smallFeatureCulling" : true,
                    "smallFeatureTreshold" : 0.01,
                    "smallFeaturePixelThreshold" : 32,
                    "resizeCanvasToVisiblePortion" : true
                }
            },
            {
                "name" : "Remote Rendering (G-Buffers)",
                "tag" : "rm_gbuffers",
                "pipelineSetup" : {},
                "settings" :
                {
                    "frustumCulling" : true,
                    "smallFeatureCulling" : true,
                    "smallFeatureTreshold" : 0.01,
                    "smallFeaturePixelThreshold" : 32,
                    "resizeCanvasToVisiblePortion" : true
                }
            }
        ]
}

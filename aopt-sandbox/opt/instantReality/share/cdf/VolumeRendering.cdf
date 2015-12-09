<?xml version="1.0" encoding="UTF-8"?>
<component level='0' library='VolumeRenderingNodePool' name='VolumeRendering' providerUrl='http://www.instantreality.org/' specWebName='volume' standard='X3D' title='VolumeRendering Component'>
  <node level='0' name='AccelerationVolumeSettings'/>
  <node level='3' name='BlendedVolumeStyle'/>
  <node level='2' name='BoundaryEnhancementVolumeStyle'/>
  <node level='3' name='CartoonVolumeStyle'/>
  <node level='1' name='ComposableVolumeRenderStyle'/>
  <node level='2' name='ComposedVolumeStyle'/>
  <node level='2' name='EdgeEnhancementVolumeStyle'/>
  <node level='2' name='ISOSurfaceVolumeData'/>
  <node level='0' name='NormalVolumeSettings'/>
  <node level='1' name='OpacityMapVolumeStyle'/>
  <node level='2' name='ProjectionVolumeStyle'/>
  <node level='0' name='RayGenerationVolumeSettings'/>
  <node level='2' name='SegmentedVolumeData'/>
  <node level='3' name='ShadedVolumeStyle'/>
  <node level='2' name='SilhouetteEnhancementVolumeStyle'/>
  <node level='2' name='ToneMappedVolumeStyle'/>
  <node level='1' name='VolumeData'/>
  <node level='1' name='VolumeNode'/>
  <node level='0' name='VolumeRenderSettings'/>
  <node level='1' name='VolumeRenderStyle'/>
  <node level='0' name='XRayVolumeStyle'/>
  <![CDATA[Volume Rendering is an alternate form of visual data representation compared to the traditional polygonal form. Where polygons represent an infinitely thin plane, volume data represents a three dimensional block of space that contains some data.There are many different techniques for implementing rendering of volumetric data. This component does not define the technique used to render the data, only the type of visual output needed. In addition, it defines several different types of data representations for which the renderings may be applied. Each of the rendering nodes will represent the visual output required, not the technique used to implement it.]]>
</component>

const width = 790;
const height = 790;
const nodeRadius = 35;
const anchorRadius = 5;

const extensionLength = 20;
const anchSpace = 80;
const pathChargeStrength = -100;
const pathLinkStrength = 2;
const distance = 80;

const color = d3.scaleOrdinal()
  .range(d3.schemeCategory20);

function remove(lst, element) {
  let index = lst.indexOf(element);
  if (index > -1) {
    lst.splice(index, 1);
  }
}


// node type
class Node {
  constructor(label, x, y, angles=[]) {
    this.type = "Node";
    this.x = x;
    this.y = y;
    this.r = nodeRadius;
    this.label = label;

    var instance = this; // this is too weird: we can't nake this "self", since that is taken by GraphDraw...
    this.anchors = [];
    angles.forEach(function(a) { instance.anchors.push( new Anchor(instance, a) ); } );
    this.centerAnchor = new CenterAnchor(this);

    this.links = [];
  }
  anchors() {
    return this.anchors.concat(this.centerAnchor);
  }
  addLink(l) {
    this.links.push(l);
  }
  rmLink(l) {
    remove(this.links, l);
  }
}

// fixed node anchor point type
class Anchor {
  constructor(owner, angle) {
    this.type = "Anchor";
    this.owner = owner;
    this.angle = angle;
    this.vx = 0;
    this.vy = 0;
    this.r = 3;
    this.localx = owner.r * Math.cos(this.angle/360*2*Math.PI);
    this.localy = - this.owner.r * Math.sin(this.angle/360*2*Math.PI); // svg inverted y-axis
    this.selected = false;

    this.ext = new ExtensionAnchor(owner, angle);
  }
  get x() { return this.owner.x + this.localx; }
  set x(value) { /* empty:)) */ }
  get y() { return this.owner.y + this.localy; }
  set y(value) { /* empty:)) */ }
}

// a radial extension to the Anchor (fixed at a relative position), as a path helper
class ExtensionAnchor {
  constructor(owner, angle) {
    this.type = "ExtensionAnchor";
    this.owner = owner; // this is the node, not the anchor
    this.angle = angle;
    this.vx = 0;
    this.vy = 0;
    this.localx = (owner.r + extensionLength) * Math.cos(this.angle/360*2*Math.PI);
    this.localy = - (this.owner.r + extensionLength) * Math.sin(this.angle/360*2*Math.PI); // svg inverted y-axis
  }
  get x() { return this.owner.x + this.localx; }
  set x(value) { /* empty:)) */ }
  get y() { return this.owner.y + this.localy; }
  set y(value) { /* empty:)) */ }
}

// path helper node type
class PathAnchor {
  constructor(x, y, owner) {
    this.owner = owner; // this must be a link object
    this.x = x;
    this.y = y;
    this.vx = 0;
    this.vy = 0;
  }
}

// placed at Node centers, used to provide a static charge for path layout simulations
class CenterAnchor {
  constructor(owner) {
    this.owner = owner;
    this.x = 0;
    this.y = 0
    this.vx = 0;
    this.vy = 0;
  }
  get x() { return new Number(this.owner.x); }
  set x(value) { /* empty:)) */ }
  get y() { return new Number(this.owner.y); }
  set y(value) { /* empty:)) */ }
}

// link data type
class Link {
  constructor(d1, d2) {
    this.d1 = d1;
    this.d2 = d2;
    this.pathAnchors = [];
    this.recalcPathAnchors();

    d1.owner.addLink(this);
    d2.owner.addLink(this);
  }
  recalcPathAnchors() {
    this.pathAnchors = [];
    let x1 = this.d1.ext.x;
    let y1 = this.d1.ext.y;
    let x2 = this.d2.ext.x;
    let y2 = this.d2.ext.y;

    let distx = Math.abs(x1 - x2);
    let disty = Math.abs(y1 - y2);
    let len = Math.sqrt( distx*distx + disty*disty );
    let xScale = d3.scaleLinear()
      .domain([0, len])
      .range([x1, x2]);
    let yScale = d3.scaleLinear()
      .domain([0, len])
      .range([y1, y2]);
    let na = Math.floor(len/anchSpace) - 1;
    let space = len/na;
    for (let i=1; i <= na; i++) {
      this.pathAnchors.push( new PathAnchor(xScale(i*space), yScale(i*space), this) );
    }
  }
  length() {
    let dx = d2.x - d1.x;
    let dy = d2.y - d1.y;
    return Math.sqrt(dx*dx + dy*dy);
  }
  getAnchors() {
    let result = [this.d1, this.d1.ext].concat(this.pathAnchors);
    result.push(this.d2.ext);
    result.push(this.d2);
    return result;
  }
  detatch() {
    this.d1.owner.rmLink(this);
    this.d2.owner.rmLink(this);
  }
  nodeNames() {
    return [this.d1.owner.label, this.d2.owner.label];
  }
}

// node data manager, keeping this interface tight and providing convenient arrays for layout sims
class GraphData {
  constructor() {
    this.nodes = [];
    this.links = [];
    this.anchors = [];
    this.forceLinks = [];

    this.nodeLabels = [];
  }
  // should be private
  updateAnchors() {
    this.anchors = [];
    this.forceLinks = [];
    for (let j = 0; j < this.links.length; j++) {

      let anchors = this.links[j].getAnchors();
      let fl = null;

      for (let i = 0; i < anchors.length; i++) {
        this.anchors.push(anchors[i]);
        if (i > 0) {
          fl = { 'source' : anchors[i-1], 'target' : anchors[i], 'index' : null }
          this.forceLinks.push(fl);
        }
      }
    }
  }
  getAnchors() {
    this.updateAnchors();
    return this.anchors;
  }
  getForceLinks() {
    this.updateAnchors();
    return this.forceLinks;
  }
  addNode(n) {
    if (!this.nodeLabels.includes(n.label)) {
      this.nodes.push(n);
      this.nodeLabels.push(n.label);
      this.anchors.push(n.centerAnchor);
    }
  }
  rmNode(n) {
    let nl = n.links.length;
    for (var i=0; i<nl; i++) {
      this.rmLink(n.links[0]);
    }
    remove(this.nodes, n);
  }
  addLink(l) {
    this.links.push(l);
    this.updateAnchors();
  }
  rmLink(l) {
    l.detatch();
    remove(this.links, l);
  }
}

// responsible for drawing, and acts as an interface
class GraphDraw {
  constructor() {
    this.graphData = new GraphData();

    this.color = d3.scaleOrdinal().range(d3.schemeCategory20);
    this.svg = d3.select('#svg_container')
      .append('svg')
      .attr('width', width)
      .attr('height', height)
    this.svg
      .on("click", function() {
        let m = d3.mouse(this)
        let svg_x = m[0];
        let svg_y = m[1];
        clickSvg(svg_x, svg_y);
      } )
    this.collideSim = d3.forceSimulation()
      .force("collide",
        d3.forceCollide(nodeRadius + 3)
        .iterations(4)
      )
      .on("tick", this.update)
      .stop();
    this.centeringSim = d3.forceSimulation()
      .force("centering",
        d3.forceCenter(width/2, height/2)
      )
      .on("tick", this.update)
      .stop();
    this.pathSim = d3.forceSimulation()
      .force("link",
        d3.forceLink()
          .strength(pathLinkStrength)
          .distance( function(d) { return distance; } )
      )
      .force("pathcharge",
        d3.forceManyBody()
          .strength(pathChargeStrength)
      )
      .on("tick", this.update)
      .stop();

    this.draggable = null;
    this.dragAnchor = null;
    this.temp = null;

    // root nodes for various item types (NOTE: the ordering matters)
    this.linkGroup = this.svg.append("g");
    this.splineGroup = this.svg.append("g");
    this.nodeGroup = this.svg.append("g");

    // specific selections
    this.nodes = null;
    this.paths = null;
    this.anchors = null;

    // pythonicism
    self = this;
  }
  addNode_obj(node) {
    self.graphData.addNode(node);
    self.resetChargeSim();
    self.restartCollideSim(); // make sure everything is centered
    self.drawNodes();
  }
  resetChargeSim() {
    // the charge force seems to have to reset like this for some reason
    self.distanceSim = d3.forceSimulation(self.graphData.nodes)
      .force("noderepulsion",
        d3.forceManyBody()
          .strength( -40 )
          .distanceMin(20)
          .distanceMax(100))
      //.on("tick", self.drawNodes)
      .on("tick", self.update)
      .stop();
  }
  resetPathSim() {
    self.pathSim.stop();
    self.pathSim.nodes(self.graphData.getAnchors());
    self.pathSim.force("link").links(self.graphData.getForceLinks());
  }
  restartPathSim() {
    // run the pathsim manually to avoid the animation
    self.pathSim.alpha(1);
    for (var i=0; i < 300; i++) {
      self.pathSim.tick();
    }
    self.update();
  }
  restartChargeSim() {
    self.distanceSim.stop();
    self.distanceSim.alpha(1).restart();
  }
  restartCollideSim() {
    self.collideSim.stop();
    self.collideSim.nodes(self.graphData.nodes);
    self.collideSim.alpha(1).restart();
    // path anchors go into the center-sim only
    self.centeringSim.stop();
    self.centeringSim.nodes(self.graphData.nodes.concat(self.graphData.anchors));
    self.centeringSim.alpha(1).restart();
  }
  dragged(d) {
    // reheating collision protection is needed during long drags
    if (self.collideSim.alpha() < 0.1) { self.restartCollideSim(); }

    d.x += d3.event.dx;
    d.y += d3.event.dy;
  }
  dragstarted(d) {
    d.active = true;
    self.restartCollideSim();
  }
  dragended(d) {
    d.active = false;

    // recalc node link path anchors here
    d.links.forEach( function(l) {
      l.recalcPathAnchors();
    } )

    // restart post-drag relevant layout sims
    self.restartChargeSim();
    self.resetPathSim(); // we need to reset, because the path anchors may have changed during recalcPathAnchors
    self.restartPathSim();

    self.drawNodes();
  }
  anchorMouseDown(d) {
    self.dragAnchor = d;
    d.selected = true;
  }
  anchorMouseUp(d) {
    let s = self.dragAnchor;
    if (s != d && s.owner != d.owner)
    {
      self.graphData.addLink(new Link(s, d));
      self.drawNodes();
    }
    d.selected = false;
    self.dragAnchor = null;

    self.resetPathSim();
    self.restartPathSim();
  }
  ctrlClickNode(n) {
    self.graphData.rmNode(n);
    self.drawNodes();
    self.restartCollideSim();
  }
  update() {
    self.draggable
      .attr("transform", function(d) { return "translate(" + d.x + "," + d.y + ")"; } );
    self.nodes
      .classed("active", function(d) { return d.active; });

    self.splines
      .each( function(l, i) {
        self.temp =  l.getAnchors()
        let anchors = self.temp;

        let g = d3.select(this)
          .select('path')
          .datum(anchors)
          .attr('d', d3.line()
            //.curve(d3.curveMonotoneX)
            .curve(d3.curveBasis)
            .x( function(p) { return p.x; } )
            .y( function(p) { return p.y; } )
          );
      });

    /*
    // for DEBUG purposes
    self.paths
      .attr("x1", function(d) { return d['source'].x; })
      .attr("y1", function(d) { return d['source'].y; })
      .attr("x2", function(d) { return d['target'].x; })
      .attr("y2", function(d) { return d['target'].y; });

    self.anchors
      .attr("cx", function(d) { return d.x; })
      .attr("cy", function(d) { return d.y; });
    */
  }

  drawNodes() {
    // clear all nodes
    if (self.draggable) self.draggable.remove();

    // draw all nodes
    self.draggable = self.nodeGroup.selectAll("g")
      .data(self.graphData.nodes)
      .enter()
      .append("g")
      .call( d3.drag()
        .filter( function() {
          return !d3.event.button && !d3.event.ctrlKey;
        })
        .on("start", self.dragstarted)
        .on("drag", self.dragged)
        .on("end", self.dragended)
      )
      .on("click", function () { self.ctrlClickNode(d3.select(this).datum()); });

    // draw anchor nodes
    self.draggable.each( function(d, i) {
      let anchors = d3.select(this)
      .append("g")
      .selectAll("circle")
      .data(d.anchors)
      .enter()
      .append("circle")
      .attr('r', anchorRadius)
      // semi-static transform, which does not belong in update()
      .attr("transform", function(p) { return "translate(" + p.localx + "," + p.localy + ")" } )
      .style("fill", "white")
      .style("stroke", "#000")
      .on("mousedown", function(p) {
        // these two lines will prevent drag behavior
        d3.event.stopPropagation();
        d3.event.preventDefault();
        self.anchorMouseDown(p)
      } )
      .on("mouseup", function(p) {
        //console.log("mouseup", self.dragAnchor != null);
        self.anchorMouseUp(p);
      } )
      .on("mouseover", function(d) {
        d3.select(this)
        .classed("active", true)
        .style("opacity", 1);
      } )
      .on("mouseout", function(d) {
        d3.select(this)
          .classed("active", false)
      } )
    });

    self.draggable.append('text')
      .text( function(d) { return d.label } )
      .attr("font-family", "sans-serif")
      .attr("font-size", "20px")
      .attr("fill", "white")
      .attr("text-anchor", "middle")
      .attr("dominant-baseline", "middle")
      .lower();

    this.nodes = self.draggable.append('circle')
      .attr('r', function(d) { return d.r; })
      .style("fill", function(d, i) { return color(i); })
      .lower();

    /*
    // draw sublines for each link
    if (self.paths) self.paths.remove();
    self.paths = self.linkGroup.selectAll("line")
      .data(self.graphData.getForceLinks())
      .enter()
      .append("line")
      .style("stroke", "black");
    */

    // draw the splines
    let links = self.graphData.links;

    if (self.splines) self.splines.remove();
    self.splines = self.splineGroup.selectAll("g")
      .data(links)
      .enter()
      .append("g");

    self.splines
      .each( function(l, i) {
        self.temp =  l.getAnchors()
        let anchors = self.temp;

        let g = d3.select(this)
          .append('path')
          .datum(anchors)
          .attr("class", "line")
          .attr('d', d3.line()
            //.curve(d3.curveMonotoneX)
            .curve(d3.curveBasis)
            .x( function(p) { return p.x; } )
            .y( function(p) { return p.y; } )
          );
      });

    /*
    let test = [{x:10, y:10}, {x:15, y:15}, {x:15, y:30}]
    self.svg.append("g").append("path")
      .datum(test)
      .attr("class", "line")
      .attr('d', d3.line()
        .curve(d3.curveBasis)
        .x( function(p) { return p.x; } )
        .y( function(p) { return p.y; } )
      );
    */

    /*
    // draw the actual anchors
    let anchors = self.graphData.anchors;
    if (self.anchors) self.anchors.remove();
    self.anchors = self.linkGroup.selectAll("circle")
      .data(anchors)
      .enter()
      .append("circle")
      .attr("cx", function(d) { return d.x; })
      .attr("cy", function(d) { return d.y; })
      .attr("r", 5)
      .attr("fill", "black");
    */

    // update data properties in a separate function
    self.update();
  }
}

let draw = null;
// entry point
function run() {
  draw = new GraphDraw();

  // example nodes
  createNode("gauss", 100, 240, [70, 90, 110, 270]);
  createNode("en1", 100, 450, [100]);
  createNode("pg", 50, 44, [270]);
  createNode("pg2", 100, 44, [270]);
  createNode("pg3", 150, 44, [270]);
}
function createNode(label, x, y, angles) {
  draw.addNode_obj( new Node(label, x, y, angles) );
}

// ui interaction
let nodeLabel = '';
let anchArray = [];
let labelHistory = []
let clearTbxCB = null;
function pushNodeLabel(label, anchArr) {
  nodeLabel = label;
  anchorArray = anchArr;
}
function clickSvg(x, y) {
  label = nodeLabel;
  nodeLabel = '';
  anchs = anchorArray;
  anchArray = [];
  if (label != '') {
    createNode(label, x, y, anchs);
    draw.drawNodes();
    if (clearTbxCB) {
      clearTbxCB();
    }
    labelHistory.push(label);
    console.log("a node with label '" + label + "' was added")
  }
}

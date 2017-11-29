const width = 790;
const height = 790;
const nodeRadius = 35;
const anchorRadius = 5;

const extensionLength = 40;
const anchSpace = 80;
const pathChargeStrength = -100;
const pathLinkStrength = 2;
const distance = 80;

const arrowHeadLength = 12;
const arrowHeadAngle = 25;

// some debug colours
const color = d3.scaleOrdinal().range(d3.schemeCategory20);

// list.remove convenience function
function remove(lst, element) {
  let index = lst.indexOf(element);
  if (index > -1) {
    lst.splice(index, 1);
  }
}

NodeIconType = {
  CIRCE : 0,
  CIRCLEPAD : 1,
  SQUARE : 2,
  FLUFFY : 3,
  FLUFFYPAD : 4,
  HEXAGONAL : 5,
}

NodeState = {
  DISCONNECTED : 0,
  PASSIVE : 1,
  ACTIVE : 2,
  RUNNING : 3,
  FAIL : 4,
}

function getNodeStateClass(state) {
  if (state==NodeState.DISCONNECTED) {
    return "disconnected";
  }
  else if (state==NodeState.PASSIVE) {
    return "passive";
  }
  else if (state==NodeState.ACTIVE) {
    return "active";
  }
  else if (state==NodeState.RUNNING) {
    return "running";
  }
  else if (state==NodeState.FAIL) {
    return "fail";
  }
  else throw "invalid value"
}

// node data type
class Node {
  constructor(label, x, y) {
    this.x = x;
    this.y = y;
    this.r = nodeRadius;
    this.label = label;
    this.anchors = [];
    this.links = [];
    this.centerAnchor = new CenterAnchor(this);

    this.state = NodeState.DISCONNECTED;
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
  draw(branch, i) {
    return branch
      .attr('stroke', "black")
      .classed(getNodeStateClass(this.state), true);
  }
}

class NodeCircular extends Node {
  constructor(label, x, y, angles=[]) {
    super(label, x, y);
    var instance = this; // this is too weird: we can't nake this "self", since that is taken by GraphDraw...
    angles.forEach(function(a) { instance.anchors.push( new AnchorCircular(instance, a) ); } );
  }
  draw(branch, i) {
    branch = super.draw(branch, i);
    return branch
      .append('circle')
      .attr('r', function(d) { return d.r; })
  }
}

class NodeCircularPad extends Node {
  constructor(label, x, y, angles=[]) {
    super(label, x, y);
    var instance = this; // this is too weird: we can't nake this "self", since that is taken by GraphDraw...
    angles.forEach(function(a) { instance.anchors.push( new AnchorCircular(instance, a) ); } );
  }
  draw(branch, i) {
    branch = super.draw(branch, i);
    branch
      .append('circle')
      .attr('r', 0.85*this.r)
      .attr('stroke', "black")
      .attr('fill', "none")
      .lower()
    branch
      .append('circle')
      .attr('r', this.r)
      .attr('stroke', "black")
      .lower()
    return branch;
  }
}

class NodeSquare extends Node {
  constructor(label, x, y, angles=[]) {
    super(label, x, y);
    this.r = 0.85 * nodeRadius; // this is now the half height/width of the square
    var instance = this;
    angles.forEach(function(a) { instance.anchors.push( new AnchorSquare(instance, a) ); } );
  }
  draw(branch, i) {
    branch = super.draw(branch, i);
    return branch
      .append("g")
      .lower()
      .append('rect')
      .attr('width', function(d) { return 2*d.r; })
      .attr('height', function(d) { return 2*d.r; })
      .attr('x', function(d) { return -d.r; })
      .attr('y', function(d) { return -d.r; })
  }
}

class NodeHexagonal extends Node {
  constructor(label, x, y, angles=[]) {
    super(label, x, y);
    this.r = 1.05 * nodeRadius;
    var instance = this;
    angles.forEach(function(a) { instance.anchors.push( new AnchorCircular(instance, a) ); } );
  }
  draw(branch, i) {
    let r = 1.1 * this.r;
    let alpha;
    let points = [];
    for (i=0; i<6; i++) {
      alpha = i*Math.PI*2/6;
      points.push( {x : r*Math.cos(alpha), y : - r*Math.sin(alpha) } );
    }
    points.push(points[0]);

    branch = super.draw(branch, i);
    return branch
      .append('path')
      .datum(points)
      .attr('d', d3.line()
        .x( function(p) { return p.x; } )
        .y( function(p) { return p.y; } )
      );
  }
}

class NodeFluffy extends Node {
  constructor(label, x, y, angles=[]) {
    super(label, x, y);
    this.numfluff = 14;
    this.fluffrad = 7;
    this.r = 1.05 * nodeRadius;
    var instance = this;
    angles.forEach(function(a) { instance.anchors.push( new AnchorCircular(instance, a) ); } );
  }
  draw(branch, i) {
    // fluff not satisfactory because the circle shows - try d3.lineRadial()
    let r = 0.80 * this.r;
    let alpha;
    let points = [];
    for (let j=0; j<this.numfluff; j++) {
      alpha = j*Math.PI*2/this.numfluff;
      points.push( {x : r*Math.cos(alpha), y : - r*Math.sin(alpha) } );
    }
    branch = super.draw(branch, i);
    branch.append("g").lower()
      .append('circle')
      .attr('r', r)
      .lower();
    branch.append("g").lower()
      .selectAll("circle")
      .data(points)
      .enter()
      .append("circle")
      .attr('r', this.fluffrad)
      .attr("transform", function(p) { return "translate(" + p.x + "," + p.y + ")" } );

    return branch;
  }
}

class NodeFluffyPad extends NodeFluffy {
  constructor(label, x, y, angles=[]) {
    super(label, x, y);
    this.r = 0.9*this.r;
    this.numfluff = 8;
    this.fluffrad = 12;
  }
  draw(branch, i) {
    branch
      .append('circle')
      .attr("fill", "none")
      .attr('r', 0.85*this.r);

    return super.draw(branch, i)
  }

}

// connection anchor point fixed on a node at a circular periphery
class Anchor {
  constructor(owner, angle) {
    this.owner = owner;
    this.angle = angle;

    this.vx = 0;
    this.vy = 0;
    this.r = 3;
    this.localx = null;
    this.localy = null;

    this.selected = false;
    this.ext = null;

    this.isTarget = false;
    this.arrowHead = null;
  }
  get x() { return this.owner.x + this.localx; }
  set x(value) { /* empty:)) */ }
  get y() { return this.owner.y + this.localy; }
  set y(value) { /* empty:)) */ }
  drawArrowhead(branch, i) {
    if (!this.isTarget) return branch;

    let angle1 = Math.PI/180*(this.angle - arrowHeadAngle);
    let angle2 = Math.PI/180*(this.angle + arrowHeadAngle);
    let x0 = this.localx + this.localx*anchorRadius/nodeRadius;
    let y0 = this.localy + this.localy*anchorRadius/nodeRadius;
    let x1 = x0 + arrowHeadLength*Math.cos(angle1);
    let y1 = y0 - arrowHeadLength*Math.sin(angle1);
    let x2 = x0 + arrowHeadLength*Math.cos(angle2);
    let y2 = y0 - arrowHeadLength*Math.sin(angle2);
    let points = [{x:x1,y:y1}, {x:x0,y:y0}, {x:x2,y:y2}]

    this.arrowHead = branch.append("path")
      .datum(points)
      .attr("class", "arrow")
      .attr('d', d3.line()
        .x( function(p) { return p.x; } )
        .y( function(p) { return p.y; } )
      );
    return this.arrowHead;
  }
}

class AnchorCircular extends Anchor {
  constructor(owner, angle) {
    super(owner, angle);
    this.localx = owner.r * Math.cos(this.angle/360*2*Math.PI);
    this.localy = - this.owner.r * Math.sin(this.angle/360*2*Math.PI); // svg inverted y-axis

    let ext_localx = (owner.r + extensionLength) * Math.cos(this.angle/360*2*Math.PI);
    let ext_localy = - (this.owner.r + extensionLength) * Math.sin(this.angle/360*2*Math.PI); // svg inverted y-axis
    this.ext = new ExtensionAnchor(owner, ext_localx, ext_localy);
  }
}

class AnchorSquare extends Anchor {
  constructor(owner, angle) {
    super(owner, angle);
    this.localx = owner.r * Math.cos(this.angle/360*2*Math.PI);
    this.localy = - this.owner.r * Math.sin(this.angle/360*2*Math.PI); // svg inverted y-axis

    // TODO: angle = angle modulus 360 and shift to positive
    if (0 <= angle && angle < 360/8) {
      this.localx = owner.r;
      this.localy = -owner.r * Math.tan(angle/180*Math.PI);
    }
    else if (360/8 <= angle && angle < 3*360/8) {
      //this.localx = owner.r * Math.cos(angle/180*Math.PI);
      this.localx = -owner.r * Math.tan(angle/180*Math.PI - Math.PI/2);
      this.localy = -owner.r;
    }
    else if (3*360/8 <= angle && angle < 5*360/8) {
      this.localx = -owner.r;
      this.localy = owner.r * Math.tan(angle/180*Math.PI - Math.PI);
    }
    else if (5*360/8 <= angle && angle < 7*360/8) {
      this.localx = owner.r * Math.tan(angle/180*Math.PI - 3*Math.PI/2);
      this.localy = owner.r;
    }
    else if (7*360/8 <= angle && angle < 360) {
      this.localx = owner.r;
      this.localy = -owner.r * Math.tan(angle/180*Math.PI);
    }

    let ext_localx = this.localx + this.localx/owner.r * extensionLength;
    let ext_localy = this.localy + this.localy/owner.r * extensionLength;
    this.ext = new ExtensionAnchor(owner, ext_localx, ext_localy);
  }
}

// a "path helper" radial extension to Anchor
class ExtensionAnchor {
  constructor(owner, localx, localy) {
    this.owner = owner; // this is the node, not the anchor
    this.vx = 0;
    this.vy = 0;
    this.localx = localx;
    this.localy = localy;
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

    d2.isTarget = true;
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
    this.d2.isTarget = false;
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

class ConnectionTruth {
  constructor() {
    // This class will tell the truth about suggested connections
    // and node states. It is the rule book and should always be
    // consulted before making changes to the node graph.
    // However, these rules are not built into the graph classes,
    // making them exchangable.
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
    self.arrowHeads = null;

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
    if (s && s != d && s.owner != d.owner)
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
      .classed("selected", function(d) { return d.active; });

    self.splines
      .each( function(l, i) {
        self.temp =  l.getAnchors()
        let anchors = self.temp;

        let g = d3.select(this)
          .select('path')
          .datum(anchors)
          .attr('d', d3.line()
            .curve(d3.curveBasis)
            .x( function(p) { return p.x; } )
            .y( function(p) { return p.y; } )
          );
      });

    /*
    // for DEBUG purposes
    self.anchors
      .attr("cx", function(d) { return d.x; })
      .attr("cy", function(d) { return d.y; });
    */
  }

  drawNodes() {
    // clear all nodes
    if (self.draggable) self.draggable.remove();

    // prepare node groups
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
      let branch = d3.select(this)
        .append("g")
        .selectAll("circle")
        .data(d.anchors)
        .enter()
        .append("g")
      branch
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
          self.anchorMouseUp(p);
        } )
        .on("mouseover", function(d) {
          d3.select(this)
            .classed("selected", true)
            .style("opacity", 1);
        } )
        .on("mouseout", function(d) {
          d3.select(this)
            .classed("selected", false)
        } )

      branch
        .each( function(d, i) {
          d.drawArrowhead(d3.select(this), i).lower();
        } );

    });
    // draw labels
    self.draggable.append('text')
      .text( function(d) { return d.label } )
      .attr("font-family", "sans-serif")
      .attr("font-size", "20px")
      .attr("color", "red")
      //.attr("fill", "black")
      .attr("text-anchor", "middle")
      .attr("dominant-baseline", "middle")
      .lower();

    // draw nodes (by delegation & strategy)
    this.nodes = self.draggable.append("g").lower().each( function(d, i) {
      d.draw(d3.select(this), i).lower();
    });

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
          .attr("class", "arrow")
          .attr('d', d3.line()
            .curve(d3.curveBasis)
            .x( function(p) { return p.x; } )
            .y( function(p) { return p.y; } )
          );
      });

    /*
    // DEBUG draw anchors
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

    // update data properties
    self.update();
  }
}

let draw = null;
// entry point
function run() {
  draw = new GraphDraw();

  // example nodes
  createNodeSquare("gauss", 100, 240, [70, 90, 110, 270]);
  createNodeHexagonal("en1", 100, 450, [100]);
  createNodeCircularPad("pg", 50, 44, [270]);
  createNodeFluffy("pg2", 100, 44, [270]);
  createNodeCircular("pg3", 150, 44, [270]);
}
function createNodeCircular(label, x, y, angles) {
  let n = new NodeCircular(label, x, y, angles);
  n.state = NodeState.DISCONNECTED;
  draw.addNode_obj( n );
}
function createNodeCircularPad(label, x, y, angles) {
  let n = new NodeCircularPad(label, x, y, angles);
  n.state = NodeState.PASSIVE;
  draw.addNode_obj( n );
}
function createNodeSquare(label, x, y, angles) {
  let n = new NodeSquare(label, x, y, angles);
  n.state = NodeState.ACTIVE;
  draw.addNode_obj( n );
}
function createNodeHexagonal(label, x, y, angles) {
  let n = new NodeHexagonal(label, x, y, angles);
  n.state = NodeState.RUNNING;
  draw.addNode_obj( n );
}
function createNodeFluffy(label, x, y, angles) {
  let n = new NodeFluffy(label, x, y, angles);
  n.state = NodeState.PASSIVE;
  draw.addNode_obj( n );
}
function createNodeFluffyPad(label, x, y, angles) {
  let n = new NodeFluffyPad(label, x, y, angles);
  n.state = NodeState.PASSIVE;
  draw.addNode_obj( n );
}

// ui interaction
let nodeLabel = '';
let anchArray = [];
let labelHistory = []
let clearTbxCB = null;
let nodeType = '';
function pushNodeLabel(label, anchArr, type) {
  nodeLabel = label;
  anchorArray = anchArr;
  nodeType = type;
}
function clickSvg(x, y) {
  label = nodeLabel;
  nodeLabel = '';
  anchs = anchorArray;
  anchArray = [];
  if (label != '') {
    if (nodeType == NodeIconType.CIRCE) { createNodeCircular(label, x, y, anchs, NodeState.DISCONNECTED); }
    else if (nodeType == NodeIconType.CIRCLEPAD) { createNodeCircularPad(label, x, y, anchs); }
    else if (nodeType == NodeIconType.SQUARE) { createNodeSquare(label, x, y, anchs); }
    else if (nodeType == NodeIconType.FLUFFY) { createNodeFluffy(label, x, y, anchs); }
    else if (nodeType == NodeIconType.FLUFFYPAD) { createNodeFluffyPad(label, x, y, anchs); }
    else if (nodeType == NodeIconType.HEXAGONAL) { createNodeHexagonal(label, x, y, anchs); }
    else { console.log("default"); }

    draw.drawNodes();
    if (clearTbxCB) {
      clearTbxCB();
    }
    labelHistory.push(label);
    console.log("a node with label '" + label + "' was added")
  }
}

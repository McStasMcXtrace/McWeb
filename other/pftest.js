const nodeRadius = 30;
const width = 790;
const height = 400;

class Node {
  constructor(label, x, y) {
    this.x = x;
    this.y = y;
    this.r = 30;
    this.label = label;
    this.active = false;
  }
}

let color = d3.scaleOrdinal()
    .range(d3.schemeCategory20);

class GraphDraw {
  constructor() {
    this.nodes = [];
    this.color = d3.scaleOrdinal().range(d3.schemeCategory20);
    this.svg = d3.select('#svg_container')
      .append('svg')
      .attr('width', width)
      .attr('height', height)
      .style("border", "1px solid black");
    this.svg
      .on("click", function(i) {
        let m = d3.mouse(this)
        let svg_x = m[0];
        let svg_y = m[1];
        clickSvg(svg_x, svg_y);
        } )
    this.collideSimulation = d3.forceSimulation(self.nodes)
        .force("collide",
          d3.forceCollide(nodeRadius + 3)
          .iterations(4))
        .on("tick", self.drawNodes);

    this.dragable = null;
    // because it's the most pythonic...
    self = this;
  }
  addNode(label, x, y) {
    self.nodes.push(new Node(label, x, y));
    self.collideSimulation.nodes(self.nodes);
  }
  drawNodes() {
    // clear all nodes
    if (self.dragable) self.dragable.remove();

    // draw all nodes
    self.dragable = self.svg.selectAll("g")
      .data(self.nodes)
      .enter()
      .append("g")
      .attr("transform", function(d) { return "translate(" + d.x + "," + d.y + ")"; } )
      .raise().classed("active", function(d) { return d.active; })
      .call(d3.drag()
        .on("start", self.dragstarted)
        .on("drag", self.dragged)
        .on("end", self.dragended));

    self.dragable.append('circle')
      .attr('r', function(d) { return d.r; })
      .style("fill", function(d, i) { return color(i); });

    self.dragable.append('text')
      .text(function(d) { return d.label })
      .attr("font-family", "sans-serif")
      .attr("font-size", "20px")
      .attr("fill", "white")
      .attr("text-anchor", "middle")
      .attr("dominant-baseline", "middle");
  }
  resetSimulation() {
    self.collideSim
      .nodes(self.nodes)
      .alpha(0.8)
      .restart();
  }
  dragged(d) {
    // function is sensitive to box edges given by width, height and nodeRadius
    // TODO: reimplement this in terms of a d3.force to fix the collide reaction issues
    d.x += d3.event.dx;
    d.y += d3.event.dy;
    if (d.x + nodeRadius < width &&
        d.y + nodeRadius < height &&
        d.x - nodeRadius > 0 &&
        d.y - nodeRadius > 0)
    {
      self.drawNodes();
    }
    else
    {
      d.x -= d3.event.dx;
      d.y -= d3.event.dy;
    }
  }
  dragstarted(d) {
    d.active = true;
    self.collideSimulation.alphaTarget(0.3).restart();
    self.drawNodes();
  }
  dragended(d) {
    d.active = false;
    self.drawNodes();
  }
}
var gd = null;
function run() {
  gd = new GraphDraw();
  gd.addNode("gauss", 100, 140);
  gd.addNode("en1", 520, 240);
  gd.addNode("pg", 450, 44);
  gd.drawNodes();
}
var nodeLabel = '';
var labelHistory = []
var clearTbxCB = null;
function pushNodeLabel(label) {
  nodeLabel = label;
}
function clickSvg(x, y)  {
  label = nodeLabel;
  nodeLabel = '';
  if (label != '') {
    gd.addNode(label, x, y);
    gd.drawNodes();
    if (clearTbxCB) {
      clearTbxCB();
    }
    labelHistory.push(label);
    console.log("a node with label '" + label + "' was added")
  }
  else {
    console.log("empty label, no node was added");
  }
}

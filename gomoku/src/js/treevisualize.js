var D3 = D3 || {};

D3.requestTree = function() {
  var request = new XMLHttpRequest();
  request.open('POST', '/visualize');

  request.onreadystatechange = function() {
    if (request.readyState != 4 || request.status != 200)
      return;

    if (request.responseText) D3.drawTree(JSON.parse(request.responseText));
  }

  request.send("requesting mct");
}

D3.removeTree = function() {
  d3.select("#tv-svg").select("g").remove();
}

D3.drawTree = function(treeData) {
  // Assigns parent, children, height, depth.
  // Sort childrens by total simulations in descendant order.
  var root = d3.hierarchy(treeData)
               .sort(function(child1, child2) {
                 return child2.data.totalCount - child1.data.totalCount;
               });

  var margin = { left: 60, top: 40, right: 60, bottom: 40};

  // append the svg object to the body of the page
  // appends a 'group' element to 'svg'
  // moves the 'group' element to the top left margin
  var svg = d3.select("#tv-svg");

  var g = svg.select("g").empty() ?
      svg.append("g").attr("transform", "translate("+ margin.left + "," + margin.top + ")") :
      svg.select("g");

  root.x0 = 0;
  root.y0 = 0;

  var idCounter = 0, duration = 600;

  var verticalSpacing, horizontalSpacing;

  update(root);

  svg.transition().duration(duration)
     .attr("width", root.height * horizontalSpacing + margin.left + margin.right)
     .attr("height", (root.leaves().length - 1) * verticalSpacing + margin.top + margin.bottom);

  function update(source) {
    var t = d3.transition().duration(duration);

    verticalSpacing = 300 / Math.pow(root.leaves().length, 1.2) + 10;

    var deepest = 0;

    // assigns leaves' x position
    root.leaves().forEach(function(d, i) {
      d.x = i * verticalSpacing ;
      if (d.depth > deepest) deepest = d.depth;
    });

    horizontalSpacing = 150 / Math.pow(deepest + 1, 1.2) + 60;

    // Compute the new tree layout.
    var nodes = root.descendants(),
        links = nodes.slice(1);

    // assings position
    nodes.forEach(function(d) {
      d.x = d.leaves()[0].x;
      d.y = d.depth * horizontalSpacing;
    });

    // ****************** Nodes section ***************************

    // Update the nodes...
    var node = g.selectAll('g.node')
        .data(nodes, function(d) {
          if (!d.id) {
            d.id = d.data.index;
            var currentNode = d;
            while (currentNode.parent) {
              currentNode = currentNode.parent;
              d.id += "-" + currentNode.data.index;
            }
          }

          return d.id;
        });

    // Enter any new nodes at the parent's previous position.
    var nodeEnter = node.enter().append('g')
        .attr('class', 'node')
        .attr("transform", function(d) {
          return "translate(" + source.y0 + "," + source.x0 + ")";
        });

    // Add Circle for the nodes
    nodeEnter.append('circle')
        .attr('r', 0)
        .style("fill", function(d) {
          return d.data.whiteTurn ? "black" : "white";
        })
        .style("stroke", function(d) {
          return gradient(d.data.winRate);
        });

    // Add labels for the nodes
    // labels will be at the right of the node,
    nodeEnter.append('text')
        .attr("x", 15)
        .attr("y", 0)
        .attr("text-anchor", "start")
        .text(function(d) {
          return d.data.index == 225 ? "(pass)" :
              ("(" + String.fromCharCode(65 + d.data.index % 15) + (Math.floor(d.data.index / 15) + 1) + ")");
        })
        .style("fill-opacity", 0);

    nodeEnter.append("line")
        .filter(function(d) { return d.data.isWinning || d.data.isLosing; })
        .attr("x1", 0)
        .attr("x2", 0)
        .attr("y1", 0)
        .attr("y2", 0);

    // UPDATE
    // Transition to the proper position for the node
    nodeEnter.merge(node).on('click', click);

    var nodeUpdate = nodeEnter.merge(node).transition(t)
        .attr("transform", function(d) {
           return "translate(" + d.y + "," + d.x + ")";
        });

    // Update the node attributes and style
    nodeUpdate.each(function(d) {
      var w = width(d.data.totalCount);

      var r = w * .75 + "px",
          cw = w * .25 + "px",
          g = gradient(d.data.winRate);

      var l = 0,
          nl = 0,
          lw = w * .2 + "px",
          s = null;

      var f = w + "px",
          dy = w * .33 + "px";

      if (d.data.isWinning || d.data.isLosing) {
        l = w * .44 + "px";
        nl = -w * .44 + "px";
        s =  d.data.isWinning ? gradient(1) : gradient(0);
      }

      var update = d3.select(this).transition(t);

      update.select('circle')
          .attr('r', r)
          .style("stroke", g)
          .style("stroke-width", cw);

      update.select('line')
          .attr("x1", nl)
          .attr("x2", l)
          .attr("y1", nl)
          .attr("y2", l)
          .style("stroke-width", lw)
          .style("stroke", s);

      update.select('text')
        .attr("dy", dy)
        .style("font-size", f)
        .style("fill-opacity", 1);
    });

    // Remove any exiting nodes
    var nodeExit = node.exit().transition(t)
        .attr("transform", function(d) {
          return "translate(" + source.y + "," + source.x + ")";
        })
        .remove();

    // On exit reduce the node circles size to 0
    nodeExit.select('circle')
        .attr('r', 0);

    // On exit reduce the opacity of text labels
    nodeExit.select('text')
        .style('fill-opacity', 0);

    nodeExit.select('line')
        .attr("x1", 0)
        .attr("x2", 0)
        .attr("y1", 0)
        .attr("y2", 0);

    // ****************** links section ***************************

    // Update the links...
    var link = g.selectAll('path.link')
        .data(links, function(d) { return d.id; });

    // Enter any new links at the parent's previous position.
    var linkEnter = link.enter().insert('path', "g")
        .attr("class", "link")
        .attr('d', function(d) {
          var o = {x: source.x0, y: source.y0};
          return diagonal(o, o);
        });

    // UPDATE
    // Transition back to the parent element position
    linkEnter.merge(link).transition(t)
        .attr('d', function(d) { return diagonal(d, d.parent); })
        .style("stroke-width", function(d) {
          return width(d.data.totalCount) + "px";
        })
        .style("stroke", function(d) {
          return gradient(d.data.winRate);
        });

    // Remove any exiting links
    link.exit().transition(t)
        .attr('d', function(d) {
          var o = {x: source.x, y: source.y};
          return diagonal(o, o);
        })
        .remove();

    // Store the old positions for transition.
    nodes.forEach(function(d) {
      d.x0 = d.x;
      d.y0 = d.y;
    });

    // Creates a curved (diagonal) path from parent to the child nodes
    function diagonal(s, d) {

      path = `M ${s.y} ${s.x}
      C ${(s.y + d.y) / 2} ${s.x},
      ${(s.y + d.y) / 2} ${d.x},
      ${d.y} ${d.x}`;

      return path;
    }

    // Toggle children on click.
    function click(d) {
      if (d.children) {
        d.hiddenChildren = d.children;
        d.children = null;
      } else {
        d.children = d.hiddenChildren;
        d.hiddenChildren = null;
      }
      update(d);
    }

    function gradient(level) {
      var hStart = 0 // red
      var hEnd = 200 //blue

      h = hStart + (hEnd - hStart) * level;
      return `hsl(${h},100%,45%)`;
    }

    function width(count) {
      return Math.pow(count, .2) + 6;
    }
  }
}
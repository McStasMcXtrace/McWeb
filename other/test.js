function testUndoRedo() {
  let und = new UndoRedoCommandStack();

  console.log(und.newdo('a', '-a'));
  console.log(und.newdo('b', '-b'));
  console.log(und.newdo('c', '-c'));
  console.log(und.newdo('d', '-d'));
  console.log(und.newdo('e', '-e'));

  console.log(und.undo());
  console.log(und.undo());
  console.log(und.undo());
  console.log(und.redo());
  console.log(und.undo());
  console.log(und.redo());

  console.log(und.newdo('x', '-x'))
  console.log(und.redo()); // this should be undefined!
  console.log(und.undo());
  console.log(und.redo());

  console.log(und.getSyncSet());
  console.log(und.getSyncSet());

  console.log(und.newdo('#', '-#'));
  console.log(und.newdo('&', '-&'));
  console.log(und.newdo('%', '-%'));

  console.log(und.getSyncSet());

  console.log(und.undo());
  console.log(und.undo());
  console.log(und.getSyncSet());
  console.log(und.getSyncSet());

  console.log(und.newdo('1', '-1'));
  console.log(und.newdo('2', '-2'));
  console.log(und.newdo('3', '-3'));
  console.log(und.undo());

  console.log(und.getSyncSet());
}
function testUndoRedoStackLimit() {
  let und = new UndoRedoCommandStack(50);

  for (var i=0; i<und.limit-5; i++) {
    und.newdo(i, -i);
  }
  console.log(und.getSyncSet());
  for (var i=und.limit-5; i<und.limit+5; i++) {
    und.newdo(i, -i);
  }

  console.log(und.ur.length);
  console.log(und.idx);
  console.log(und.synced);

  console.log(und.getSyncSet());
  und.undo();
  und.undo();
  und.undo();
  und.undo();
  console.log(und.getSyncSet());
  console.log(und.getSyncSet());

  console.log(und.ur.length);
  console.log(und.idx);
  console.log(und.synced);
}
function testUndoRedoDataBuffer() {
  let und = new UndoRedoCommandStack();

  function doit() {
    console.log(und.newdo('a', '-a'));
    console.log(und.newdo('b', '-b'));
    console.log(und.newdo('c', '-c'));
    console.log(und.newdo('d', '-d'));
    console.log(und.newdo('e', '-e'));

    console.log(und.getSyncSet()); // set sync position to latest

    console.log(und.undo()); // move to a parallel universe
    console.log(und.undo());
    console.log(und.undo());
    console.log(und.newdo('x', '-x'));

    console.log(und.getSyncSet()); // new sync set should include backtracked undos
  }
  doit();
  console.log("");
  console.log("");
  doit();
}


function drawTestNodesFormally() {
  // construct example graph using the "informal" interface
  let n1 = intface.node_add(480, 128, "o0", "", "data", "obj");
  let n2 = intface.node_add(290, 250, "o1", "", "pg", "obj");
  let n3 = intface.node_add(143, 346, "o2", "", "pc", "obj");
  let n4 = intface.node_add(336, 610, "o3", "", "plt_c", "obj");
  let n5 = intface.node_add(539, 516, "o4", "", "plt_fit", "obj");
  let n6 = intface.node_add(443, 568, "o5", "", "plt_g", "obj");

  let n7 = intface.node_add(390, 63, "f0", "", "load", "func_load");

  let n8 = intface.node_add(208, 449,  "f1", "", "const", "ifunc_const");
  let n9 = intface.node_add(311, 379, "f2", "", "gauss", "ifunc_gauss");

  // A special case - the formal node graph interface can only be used with
  // definite, "finished" node types, while we here construct a special one.
  // Supposedly custom typing could be handled by a ui widget
  conf = getConfClone("ifunc_custom");
  conf.label = "fitfunc";
  conf.itypes = ['IData'];
  conf.otypes = ['IData'];
  let n10 = intface.addNode('', conf, 565, 355);

  let n11 = intface.node_add(415, 433, "ft0", "", "+", "functional_plus");

  intface.link_add(n7.id, 0, n1.id, 0);
  intface.link_add(n3.id, 0, n8.id, 0);

  intface.link_add(n1.id, 0, n8.id, 1);
  intface.link_add(n1.id, 0, n9.id, 1);
  intface.link_add(n1.id, 0, n10.id, 0);

  intface.link_add(n8.id, 0, n11.id, 0, 1);
  intface.link_add(n9.id, 0, n11.id, 1, 1);
  intface.link_add(n11.id, 0, n10.id, 0, 1);

  intface.link_add(n8.id, 0, n4.id, 0);
  intface.link_add(n10.id, 0, n5.id, 0);
  intface.link_add(n9.id, 0, n6.id, 0);

  intface.link_add(n2.id, 0, n9.id, 0);

  //
  intface.updateUi();
}

function drawTestNodesByGraphDefinition() {
  text = '{"nodes":{"o0":[480,128,"o0","","data","obj"],"o1":[290,250,"o1","","pg","obj"],"o2":[143,346,"o2","","pc","obj"],"o3":[336,610,"o3","","plt_c","obj"],"o4":[539,516,"o4","","plt_fit","obj"],"o5":[443,568,"o5","","plt_g","obj"],"f0":[390,63,"f0","","load","func_load"],"if0":[208,449,"if0","","const","ifunc_const"],"if1":[311,379,"if1","","gauss","ifunc_gauss"],"if2":[565,355,"if2","","fitfunc","ifunc_custom"],"op0":[415,433,"op0","","+","functional_plus"]},"links":{"o0":[["o0",0,"if0",1,0],["o0",0,"if1",1,0],["o0",0,"if2",0,0]],"o1":[["o1",0,"if1",0,0]],"o2":[["o2",0,"if0",0,0]],"f0":[["f0",0,"o0",0,0]],"if0":[["if0",0,"op0",0,1],["if0",0,"o3",0,0]],"if1":[["if1",0,"op0",1,1],["if1",0,"o5",0,0]],"if2":[["if2",0,"o4",0,0]],"op0":[["op0",0,"if2",0,1]]}}';
  let def = JSON.parse(text);
  intface.injectGraphDefinition(def);
}

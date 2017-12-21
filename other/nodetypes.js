// the ultimate node types json
var nodeTypes = {
  "obj" : {
    "type": "obj",
    "basetype": "object_base",
    "itypes": [],
    "otypes": [],
    "data": null,
    "static": false,
    "name": "",
    "label": ""
  },
  "func_load" : {
    "type": "func_load",
    "basetype": "function_base",
    "itypes": [],
    "otypes": [
      "IData"
    ],
    "data": "load",
    "static": true,
    "name": "",
    "label": "load"
  },
  "ifunc_const" : {
    "type": "ifunc_const",
    "basetype": "ifunc_base",
    "itypes": [
      "pars",
      "IData"
    ],
    "otypes": [
      "IData"
    ],
    "data": "const",
    "static": true,
    "name": "",
    "label": "const"
  },
  "ifunc_gauss" : {
    "type": "ifunc_gauss",
    "basetype": "ifunc_base",
    "itypes": [
      "pars",
      "IData"
    ],
    "otypes": [
      "IData"
    ],
    "data": "gauss",
    "static": true,
    "name": "",
    "label": "gauss"
  },
  "ifunc_custom" : {
    "type": "ifunc_custom",
    "basetype": "ifunc_base",
    "itypes": [],
    "otypes": [],
    "data": null,
    "static": false,
    "name": "",
    "label": ""
  },
  "functional_plus" : {
    "type": "functional_plus",
    "basetype": "functional_base",
    "itypes": [
      "IFunc",
      "IFunc"
    ],
    "otypes": [
      "IFunc"
    ],
    "data": null,
    "static": true,
    "name": "",
    "label": "+"
  },
}

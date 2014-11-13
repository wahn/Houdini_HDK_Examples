/*
 * Copyright (c) 2014
 *	Side Effects Software Inc.  All rights reserved.
 *
 * Redistribution and use of Houdini Development Kit samples in source and
 * binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. The name of Side Effects Software may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *----------------------------------------------------------------------------
 */

//****************************************************************************
//****************************************************************************
//****************************************************************************
//
// WARNING
//
// This example is deprecated, and is largely replaced by the inlinecpp
// Python module.  Except for the ability to raise exceptions, you can
// implement this example with the following Python code:
//
//     inlinecpp.extendClass(
//         hou.ObjNode,
//         "node_methods",
//         includes="#include <UT/UT_UndoManager.h>",
//         function_sources=["""
//     void setSelectable(OBJ_Node *obj_node, int selectable)
//     {
//         if (!obj_node->canAccess(PRM_WRITE_OK))
//             return; 
//     
//         UT_AutoUndoBlock undo_block("Setting selectable flag", ANYLEVEL);
//         obj_node->setPickable(selectable != 0);
//     }           
//     """])
//
//****************************************************************************
//****************************************************************************
//****************************************************************************

// This sample shows how to extend HOM using C++.  It adds a
// hou.ObjNode.setSelectable method that makes an object node selectable/
// unselectable in the viewport.

// Include Houdini's equivalent of python/Python.h.  It creates wrappers of
// Python's C API functions, so that Houdini can determine at runtime which
// version of Python to use.  This file needs to be included first to avoid
// gcc warnings.  See PY_CPythonAPI.h for more information about the API
// wrapper.  Note that you are limited to the Python API functions wrapped by
// PY_CPythonAPI.h.
#include <PY/PY_CPythonAPI.h>

// This file is needed for all HDK plugins:
#include <UT/UT_DSOVersion.h>

// We include this file only to get the forward declaration for
// HOMextendLibrary().  See below for details.
#include <HOM/HOM_Module.h>

// This file contains functions that will run arbitrary Python code:
#include <PY/PY_Python.h>

// The class defined in this file acquires the GIL within Houdini's Python
// runtime environment:
#include <PY/PY_InterpreterAutoLock.h>

// This file defines a convenience class to automatically decrement Python
// reference counts:
#include <PY/PY_AutoObject.h>

// This class in this file is Houdini's internal representation of an object
// node:
#include <OBJ/OBJ_Node.h>

// This file defines OPgetDirector(), which returns the root ("/") node.
#include <OP/OP_Director.h>

// This file defines a function to demangle a C++ class name from the result
// returned by type_info::name():
#include <UT/UT_SysSpecific.h>

// This file lets us start a undo block.
#include <UT/UT_UndoManager.h>

static void
ObjNode_setSelectable(const char *node_path, bool selectable)
    throw(HOM_OperationFailed, HOM_PermissionError)
{
    // This is the function that does the actual work.  It takes a path
    // to the object node and a flag to say if it should be selectable.
    //
    // Look up the OBJ_Node for the path.
    OP_Node *op_node = OPgetDirector()->findNode(node_path);
    if (!op_node)
	throw HOM_OperationFailed("Internal error (could not find node)");

    OBJ_Node *obj_node = op_node->castToOBJNode();
    if (!obj_node)
	throw HOM_OperationFailed("Internal error (node is not an object)");

    // Now that we have the OBJ_Node object, we can access anything exposed
    // by the HDK.  For this example, we simply need to call setPickable().
    // First, though, we create an undo block, so that any operations called
    // by setPickable will be put into this block.
    UT_AutoUndoBlock undo_block("Setting selectable flag", ANYLEVEL);

    // If this node is inside a locked asset, we want to raise a Python
    // hou.PermissionError exception.  To do that, we simply throw an instance
    // of the C++ HOM_PermissionError class.
    if (!obj_node->canAccess(PRM_WRITE_OK))
	throw HOM_PermissionError();

    obj_node->setPickable(selectable);
}

static PY_PyObject *
createHouException(
    const char *exception_class_name, const char *instance_message,
    PY_PyObject *&exception_class)
{
    // Create an instance of the given exception class from the hou
    // module, passing the instance message into the exeption class's
    // __init__ method.  This function returns a new exception instance, or
    // NULL if an error occurred.  The class is returned in exception_class
    // and is a borrowed reference.
    exception_class = NULL;

    // Note that a PY_AutoObject class is just a wrapper around a
    // PY_PyObject pointer that will call PY_Py_XDECREF when the it's destroyed.
    // We use it for Python API functions that return new object instances.
    // Because this HDK extension is installed after the hou module is
    // imported, we can be sure that we can be sure hou_module won't be null.
    PY_AutoObject hou_module(PY_PyImport_ImportModule("hou"));

    // Look up the exception by name in the module's dictionary.  Note that
    // PY_PyModule_GetDict returns a borrowed reference and that it never
    // returns NULL.  PY_PyDict_GetItemString also returns a borrowed
    // reference.
    PY_PyObject *hou_module_dict = PY_PyModule_GetDict(hou_module);
    exception_class = PY_PyDict_GetItemString(
	hou_module_dict, exception_class_name);

    // PY_PyDict_GetItemString doesn't set a Python exception, so we are careful
    // to set it ourselves if the class name isn't valid.
    if (!exception_class)
    {
	PY_PyErr_SetString(
	    PY_PyExc_RuntimeError(),
	    "Could not find exception class in hou module");
	return NULL;
    }

    // Create an instance of the exception.  First create a tuple containing
    // the arguments to __init__.
    PY_AutoObject args(PY_Py_BuildValue("(s)", instance_message));
    if (!args)
	return NULL;

    return PY_PyObject_Call(exception_class, args, /*kwargs=*/NULL);
}

static PY_PyObject *
ObjNode_setSelectable_Wrapper(PY_PyObject *self, PY_PyObject *args)
{
    // This is a wrapper that is called from the Python runtime engine.  It
    // translates the Python arguments to C/C++ ones, calls a function to do
    // the actual work, and converts exceptions raised by that function into
    // Python exceptions.
    //
    // Note that you could also use swig to automatically generate wrapper
    // functions like this.
    //
    // Since this function is called from the Python runtime engine, we
    // don't need to manually acquire the Python global interpreter lock (GIL).

    // First extract the arguments: a string and an integer (bool).
    const char *node_path;
    int selectable;
    if (!PY_PyArg_ParseTuple(args, "si", &node_path, &selectable))
	return NULL;

    // Now call ObjNode_setSelectable to do the actual work, taking care
    // of the locking and exception handling here.
    try
    {
	// If this code is called from a thread other than the main thread,
	// creating a HOM_AutoLock instance will lock, waiting until Houdini
	// is sitting idle in its event loop.  This way, we can be sure that
	// any code that accesses Houdini's internal state is threadsafe.
	HOM_AutoLock hom_lock;

	// Call the wrapped function to do the actual work.
	ObjNode_setSelectable(node_path, (bool)selectable);

	// Return PY_Py_None to indicate that no error occurred.  If your
	// wrapped function returns a value, you'll need to convert it into
	// a Python object here.
	return PY_Py_None();
    }
    catch (HOM_Error &error)
    {
	// The exceptions used by the hou module are subclasses of HOM_Error
	// (and can be found in HOM_Errors.h).  We use RTTI to get the class
	// name, remove the "HOM_" prefix, and look up the corresponding
	// exception class in the hou Python module.
	std::string exception_class_name = UTunmangleClassNameFromTypeIdName(
	    typeid(error).name());
	if (exception_class_name.find("HOM_") == 0)
	    exception_class_name = exception_class_name.substr(4);

	// Note that a PY_AutoObject class is just a wrapper around a
	// PY_PyObject pointer that will call PY_Py_XDECREF when the it's
	// destroyed.
	PY_PyObject *exception_class;
	PY_AutoObject exception_instance(createHouException(
	    exception_class_name.c_str(), error.instanceMessage().c_str(),
	    exception_class));
	if (!exception_instance)
	    return NULL;

	// Set the exception and return NULL so Python knows an exception was
	// raised.
	PY_PyErr_SetObject(exception_class, exception_instance);
	return NULL;
    }
}

void
HOMextendLibrary()
{
    // This function installs the C++ HOM extension.  When the hou module is
    // first imported, Houdini will call functions named HOMextendLibrary in
    // HDK dso's.  This function is declared in an extern "C" in HOM_Module.h.

    {
	// A PY_InterpreterAutoLock will grab the Python global interpreter
	// lock (GIL).  It's important that we have the GIL before making
	// any calls into the Python API.
	PY_InterpreterAutoLock interpreter_auto_lock;

	// We'll create a new module named "_hom_extensions", and add functions
	// to it.  We don't give a docstring here because it's given in the
	// Python implementation below.
	static PY_PyMethodDef hom_extension_methods[] = {
	    {"ObjNode_setSelectable", ObjNode_setSelectable_Wrapper,
		PY_METH_VARARGS(), ""},
	    { NULL, NULL, 0, NULL }
	};

	PY_Py_InitModule("_hom_extensions", hom_extension_methods);
    }

    // Run some Python code to add a method to the hou.ObjectNode class that
    // will call our custom function.  We create a Python function
    // that takes in a hou.ObjNode instance.  That function calls path()
    // on the instance to get the full path to the object node, and passes
    // that path and other arguments to the function in the _hom_extensions
    // module.  Then that function is assigned to a method in hou.ObjNode
    // and the function's name is removed from the global dictionary.
    PYrunPythonStatementsAndExpectNoErrors(
	"def _setSelectable(self, selectable):\n"
	"    '''Make this node selectable/unselectable in the viewport\n"
	"       and network editor.'''\n"
	"    import _hom_extensions\n"
	"    _hom_extensions.ObjNode_setSelectable(self.path(), selectable)\n"
	"__import__('hou').ObjNode.setSelectable = _setSelectable\n"
	"del _setSelectable\n");
}


/***************************************************************************
 *   Copyright (c) 2014 Abdullah Tahiri <abdullah.tahiri.yo@gmail.com      *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#ifndef _PreComp_
# include <gp_Hypr.hxx>
# include <Geom_Hyperbola.hxx>
# include <GC_MakeArcOfHyperbola.hxx>
# include <GC_MakeHyperbola.hxx>
# include <Geom_TrimmedCurve.hxx>
#endif

#include "Mod/Part/App/Geometry.h"
#include "ArcOfHyperbolaPy.h"
#include "ArcOfHyperbolaPy.cpp"
#include "HyperbolaPy.h"
#include "OCCError.h"

#include <Base/GeometryPyCXX.h>
#include <Base/VectorPy.h>

using namespace Part;

extern const char* gce_ErrorStatusText(gce_ErrorType et);

// returns a string which represents the object e.g. when printed in python
std::string ArcOfHyperbolaPy::representation(void) const
{
    Handle_Geom_TrimmedCurve trim = Handle_Geom_TrimmedCurve::DownCast
        (getGeomArcOfHyperbolaPtr()->handle());
    Handle_Geom_Hyperbola hyperbola = Handle_Geom_Hyperbola::DownCast(trim->BasisCurve());

    gp_Ax1 axis = hyperbola->Axis();
    gp_Dir dir = axis.Direction();
    gp_Pnt loc = axis.Location();
    Standard_Real fMajRad = hyperbola->MajorRadius();
    Standard_Real fMinRad = hyperbola->MinorRadius();
    Standard_Real u1 = trim->FirstParameter();
    Standard_Real u2 = trim->LastParameter();
    
    gp_Dir normal = hyperbola->Axis().Direction();
    gp_Dir xdir = hyperbola->XAxis().Direction();
    
    gp_Ax2 xdirref(loc, normal); // this is a reference XY for the hyperbola
    
    Standard_Real fAngleXU = -xdir.AngleWithRef(xdirref.XDirection(),normal);
    

    std::stringstream str;
    str << "ArcOfHyperbola (";
    str << "MajorRadius : " << fMajRad << ", "; 
    str << "MinorRadius : " << fMinRad << ", ";
    str << "AngleXU : " << fAngleXU << ", ";
    str << "Position : (" << loc.X() << ", "<< loc.Y() << ", "<< loc.Z() << "), "; 
    str << "Direction : (" << dir.X() << ", "<< dir.Y() << ", "<< dir.Z() << "), "; 
    str << "Parameter : (" << u1 << ", " << u2 << ")"; 
    str << ")";

    return str.str();
}

PyObject *ArcOfHyperbolaPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of ArcOfHyperbolaPy and the Twin object 
    return new ArcOfHyperbolaPy(new GeomArcOfHyperbola);
}

// constructor method
int ArcOfHyperbolaPy::PyInit(PyObject* args, PyObject* kwds)
{
    PyObject* o;
    double u1, u2;
    PyObject *sense=Py_True;
    if (PyArg_ParseTuple(args, "O!dd|O!", &(Part::HyperbolaPy::Type), &o, &u1, &u2, &PyBool_Type, &sense)) {
        try {
            Handle_Geom_Hyperbola hyperbola = Handle_Geom_Hyperbola::DownCast
                (static_cast<HyperbolaPy*>(o)->getGeomHyperbolaPtr()->handle());
            GC_MakeArcOfHyperbola arc(hyperbola->Hypr(), u1, u2, PyObject_IsTrue(sense) ? Standard_True : Standard_False);
            if (!arc.IsDone()) {
                PyErr_SetString(PartExceptionOCCError, gce_ErrorStatusText(arc.Status()));
                return -1;
            }

            getGeomArcOfHyperbolaPtr()->setHandle(arc.Value());
            return 0;
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PartExceptionOCCError, e->GetMessageString());
            return -1;
        }
        catch (...) {
            PyErr_SetString(PartExceptionOCCError, "creation of arc failed");
            return -1;
        }
    }
    
    // All checks failed
    PyErr_SetString(PyExc_TypeError,
        "ArcOfHyperbola constructor expects an hyperbola curve and a parameter range");
    return -1;
}

Py::Float ArcOfHyperbolaPy::getMajorRadius(void) const
{
    return Py::Float(getGeomArcOfHyperbolaPtr()->getMajorRadius()); 
}

void  ArcOfHyperbolaPy::setMajorRadius(Py::Float arg)
{
    getGeomArcOfHyperbolaPtr()->setMajorRadius((double)arg);
}

Py::Float ArcOfHyperbolaPy::getMinorRadius(void) const
{
    return Py::Float(getGeomArcOfHyperbolaPtr()->getMinorRadius()); 
}

void  ArcOfHyperbolaPy::setMinorRadius(Py::Float arg)
{
    getGeomArcOfHyperbolaPtr()->setMinorRadius((double)arg);
}

Py::Float ArcOfHyperbolaPy::getAngleXU(void) const
{
    return Py::Float(getGeomArcOfHyperbolaPtr()->getAngleXU()); 
}

void ArcOfHyperbolaPy::setAngleXU(Py::Float arg)
{
    getGeomArcOfHyperbolaPtr()->setAngleXU((double)arg);
}

Py::Object ArcOfHyperbolaPy::getCenter(void) const
{
    return Py::Vector(getGeomArcOfHyperbolaPtr()->getCenter());
}

void  ArcOfHyperbolaPy::setCenter(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d loc = static_cast<Base::VectorPy*>(p)->value();
        getGeomArcOfHyperbolaPtr()->setCenter(loc);
    }
    else if (PyObject_TypeCheck(p, &PyTuple_Type)) {
        Base::Vector3d loc = Base::getVectorFromTuple<double>(p);
        getGeomArcOfHyperbolaPtr()->setCenter(loc);
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Object ArcOfHyperbolaPy::getAxis(void) const
{
    Handle_Geom_TrimmedCurve trim = Handle_Geom_TrimmedCurve::DownCast
        (getGeomArcOfHyperbolaPtr()->handle());
    Handle_Geom_Hyperbola hyperbola = Handle_Geom_Hyperbola::DownCast(trim->BasisCurve());
    gp_Ax1 axis = hyperbola->Axis();
    gp_Dir dir = axis.Direction();
    return Py::Vector(Base::Vector3d(dir.X(), dir.Y(), dir.Z()));
}

void  ArcOfHyperbolaPy::setAxis(Py::Object arg)
{
    PyObject* p = arg.ptr();
    Base::Vector3d val;
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        val = static_cast<Base::VectorPy*>(p)->value();
    }
    else if (PyTuple_Check(p)) {
        val = Base::getVectorFromTuple<double>(p);
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }

    Handle_Geom_TrimmedCurve trim = Handle_Geom_TrimmedCurve::DownCast
        (getGeomArcOfHyperbolaPtr()->handle());
    Handle_Geom_Hyperbola hyperbola = Handle_Geom_Hyperbola::DownCast(trim->BasisCurve());
    try {
        gp_Ax1 axis;
        axis.SetLocation(hyperbola->Location());
        axis.SetDirection(gp_Dir(val.x, val.y, val.z));
        hyperbola->SetAxis(axis);
    }
    catch (Standard_Failure) {
        throw Py::Exception("cannot set axis");
    }
}

Py::Object ArcOfHyperbolaPy::getHyperbola(void) const
{
    Handle_Geom_TrimmedCurve trim = Handle_Geom_TrimmedCurve::DownCast
        (getGeomArcOfHyperbolaPtr()->handle());
    Handle_Geom_Hyperbola hyperbola = Handle_Geom_Hyperbola::DownCast(trim->BasisCurve());
    return Py::Object(new HyperbolaPy(new GeomHyperbola(hyperbola)), true);
}

PyObject *ArcOfHyperbolaPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int ArcOfHyperbolaPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}
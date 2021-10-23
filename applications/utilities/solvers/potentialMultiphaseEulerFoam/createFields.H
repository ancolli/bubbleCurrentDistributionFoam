#include "createRDeltaT.H"
#include "readGravitationalAcceleration.H"
#include "readhRef.H"

Info<< "Creating phaseSystem\n" << endl;

autoPtr<phaseSystem> fluidPtr
(
    phaseSystem::New(mesh)
);
phaseSystem& fluid = fluidPtr();
phaseSystem::phaseModelList& phases = fluid.phases();

dimensionedScalar pMin
(
    "pMin",
    dimPressure,
    fluid
);

#include "gh.H"

volScalarField& p = phases[0].thermoRef().p();

Info<< "Reading field p_rgh\n" << endl;
volScalarField p_rgh
(
    IOobject
    (
        "p_rgh",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);
///////*------------------------------------------*/////////////////////

// * * * * * * * * * * * * * Reading Properties * * * * * * * * * * * * * * * * //

Info<< "Reading transportProperties\n" << endl;

IOdictionary transportProperties
(
    IOobject
    (
        "transportProperties",
        runTime.constant(),
        mesh,
        IOobject::MUST_READ_IF_MODIFIED,
        IOobject::NO_WRITE
    )
);

Info<< "Reading electric conductivity kf\n" << endl;

dimensionedScalar kf
(
    transportProperties.lookup("kf")
);

    volScalarField keff
    (
        IOobject
        (
            "keff",
            runTime.timeName(),
            mesh,
            IOobject::READ_IF_PRESENT,//MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar("keff", kf)
    );
    
    Info<< "Reading Faraday constant F\n" << endl;

dimensionedScalar F
(
    transportProperties.lookup("F")
);

Info<< "Reading controlProperties\n" << endl;

IOdictionary controlProperties
(
    IOobject
    (
        "controlProperties",
        runTime.constant(),
        mesh,
        IOobject::MUST_READ_IF_MODIFIED,
        IOobject::NO_WRITE
    )
);

IOdictionary Results
(
    IOobject
    (
        "Results",
        runTime.constant(),
        mesh,
        IOobject::MUST_READ_IF_MODIFIED,
        IOobject::AUTO_WRITE
    )
);
    

Info<< "Reading field fi\n" << endl;

    volScalarField fi
    (
        IOobject
        (
            "fi",
            runTime.timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh
    );
      
    volScalarField Flux
    (
        IOobject
        (
            "Flux",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE//AUTO_WRITE
        ),
        mesh,
	dimensionedScalar("0", dimensionSet(0,-2,0,0,0,1,0), 0.0)
    );

///////*------------------------------------------*/////////////////////

label pRefCell = 0;
scalar pRefValue = 0.0;
if (fluid.incompressible())
{
    p = max(p_rgh + fluid.rho()*gh, pMin);

    if (p_rgh.needReference())
    {
        setRefCell
        (
            p,
            p_rgh,
            pimple.dict(),
            pRefCell,
            pRefValue
        );

        p += dimensionedScalar
        (
            "p",
            p.dimensions(),
            pRefValue - getRefCellValue(p, pRefCell)
        );
        p_rgh = p - fluid.rho()*gh;
    }
}
mesh.setFluxRequired(p_rgh.name());

PtrList<volScalarField> rAUs;
PtrList<surfaceScalarField> rAUfs;

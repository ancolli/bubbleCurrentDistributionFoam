{
// update keff using alpha
phaseModel& phase = phases[0];//phaseModel& phase = fluid.anisothermalPhases()[0]; //0 = air
const volScalarField& alpha = phase;

Info<< "Min/max alpha:" << min(alpha).value() << ' ' << max(alpha).value() << endl;

dimensionedScalar Min_kf("Min_kf",dimensionSet(-1,-3,3,0,0,2,0), SMALL); //[-1 -3 3 0 0 2 0] 

keff == max(Min_kf, kf*Foam::pow(1-alpha, 1.5)); //1.5//Min_kf in order to avoid crashing when alpha = 1

//potential field
    fi.storePrevIter();
    solve
    (
       - fvm::laplacian(keff, fi)
    );

    fi.relax();

Info<< "Min/max fi:" << min(fi).value() << ' '
    << max(fi).value() << endl;

//check if wall_anode exist
string patchNameNeeded = "wall_anode";
scalar patchOK = 0;

    forAll(mesh.boundaryMesh(), p_ith)
    {     
      string patchName = mesh.boundaryMesh()[p_ith].name();	
      if (patchNameNeeded == patchName)  // checks if "wall_anode" is in the current patchName
      {
        patchOK = 1;
      }
    }  

if (patchOK==1)  // checks if "wall_anode" is in the current patchName
      {

// if wall_anode exists then: Calculates flux on the anode

        label patchi = mesh.boundaryMesh().findPatchID("wall_anode");   
        Flux.boundaryFieldRef()[patchi] = keff.boundaryFieldRef()[patchi]*fi.boundaryFieldRef()[patchi].snGrad(); 
	label patchiC = mesh.boundaryMesh().findPatchID("wall_cathode");   
        Flux.boundaryFieldRef()[patchiC] = keff.boundaryFieldRef()[patchiC]*fi.boundaryFieldRef()[patchiC].snGrad();
        	    
// Calculates average Flux
	
        scalar sumField = 0.0;
        scalar sumField_C = 0.0;
        
        {
        	sumField = gSum
                (
                	mesh.magSf().boundaryField()[patchi]
                      * Flux.boundaryField()[patchi]
                );
        }
        
        {
        	sumField_C = gSum
                (
                	mesh.magSf().boundaryField()[patchiC]
                      * Flux.boundaryField()[patchiC]
                );
        }

        Info<< "Total current at the anode= " << sumField << " A "<< endl;
        Info<< "Total current at the cathode= " << sumField_C << " A "<< endl;
// reading and writing probeLocation (reference electrode)

        label cellNo = mesh.findCell(vector(controlProperties.lookup("refLocation")));
	scalar Vref = fi[cellNo];

// calculate cell potential
        scalar mult = 0.0;
        scalar Vcell_old = 0.0;
        scalar var = 0.0;
        scalar Vcell = readScalar(Results.lookup("Vcell")); 

        scalar control = readScalar(controlProperties.lookup("control"));  
        scalar I = readScalar(controlProperties.lookup("I"));
        scalar maxPot = readScalar(controlProperties.lookup("maxPot"));
        scalar alpha = readScalar(controlProperties.lookup("alpha"));
        scalar tol_I = readScalar(controlProperties.lookup("tol_I"));

        scalar ba = readScalar(transportProperties.lookup("ba"));

// potentiostatic mode
        scalar ref = readScalar(controlProperties.lookup("ref"));
        scalar maxvar = readScalar(controlProperties.lookup("maxvar"));
        scalar tol_ref = readScalar(controlProperties.lookup("tol_ref"));

        var=(ref < 0.0) ? -ref-Vref : ref-(Vcell-Vref); // <0 cathodic overpotential, >0 anodic overpotential//var = ref-Vref;
        var=(var>0) ? min(var,maxvar) : max(var,-maxvar);
        var=(mag(var/ref) > tol_ref) ? var : 0; // limito ref a x%

// galvanostatic
    
        mult = (mag(sumField-I)/I > tol_I) ? (sumField-I)/(I/ba) : 0.0; // limito corriente a x%

// overrelaxation 
        Vcell_old = Vcell;
	Vcell = (control==1) ? Vcell+var : ((control==2) ? maxPot : Vcell-mult);//Vcell = Vcell-mult;//
        
        Vcell = Vcell_old+alpha*(Vcell-Vcell_old);

	Vcell = max(min(Vcell, maxPot),mag(ref)); // limited potentials

// writing Vcell, I and Vref         
        Results.set("Vcell", Vcell);
        Results.set("I", sumField); 
	Results.set("Vref", (ref < 0.0) ? -Vref : (Vcell-Vref));//  
	
	Info<< "Cell voltage = " << Vcell << " V "<< endl;
   }
   else
   {
	FatalErrorIn
        (
            " Foam::patchProbes::findElements(const fvMesh&)"
        )   << " No patch name: wall_anode found "
            << " the patch name: wall_anode should exist "
            << endl
            << exit(FatalError);     
   }
}

// ************************************************************************* //

========================================================================
    MayaPlugInWizard : "rayZapper" Project Overview
========================================================================

MayaPlugInWizard has created this "rayZapper" project for you as a starting point.

This file contains a summary of what you will find in each of the files that make up your project.

MayaPlugInWizard.vcproj
    This is the main project file for projects generated using an Application Wizard. 
    It contains information about the version of the product that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

Options.txt
	This text file explains which options you selected for your new project.

/////////////////////////////////////////////////////////////////////////////
Other notes:

/////////////////////////////////////////////////////////////////////////////




/*
proc addRayZapperNodeOffsetAttrs(string $nodeName)
{

	string $nodeType = `nodeType $nodeName`;

	if($nodeType != "rayZapperNode")
	{
		error "Node is not a RayZapperNode";
	}




	int $indeces[] = `getAttr -mi ($nodeName + ".inTransform")`;
	print("Indeces = "); print($indeces);

	int $size = size($indeces);
	print ("Size of array = "); print($size + "\n");


	int $i;

	for($i = 0; $i < $size; $i++)
	{
		
		if(!`attributeExists ("inPosition" + $indeces[$i]) $nodeName`)
		{			
			addAttr -ln ("inPosition" + $indeces[$i])  -at double3  $nodeName;
			addAttr -ln ("inPosition" + $indeces[$i] + "X")  -at double -p ("inPosition" + $indeces[$i])  $nodeName;
			addAttr -ln ("inPosition" + $indeces[$i] + "Y")  -at double -p ("inPosition" + $indeces[$i])  $nodeName;
			addAttr -ln ("inPosition" + $indeces[$i] + "Z")  -at double -p ("inPosition" + $indeces[$i])  $nodeName;

			connectAttr -f ($nodeName + ".inPosition" + $indeces[$i]) ($nodeName + ".inTransform[" + $indeces[$i] + "].inPosition");

			addAttr -ln ("direction" + $indeces[$i])  -at double3  $nodeName;
			addAttr -ln ("direction" + $indeces[$i] + "X")  -at double -dv  0 -p ("direction" + $indeces[$i])  $nodeName;
			addAttr -ln ("direction" + $indeces[$i] + "Y")  -at double -dv -1 -p ("direction" + $indeces[$i])  $nodeName;
			addAttr -ln ("direction" + $indeces[$i] + "Z")  -at double -dv 0 -p ("direction" + $indeces[$i])  $nodeName;

			connectAttr -f ($nodeName + ".direction" + $indeces[$i]) ($nodeName + ".inTransform[" + $indeces[$i] + "].direction");
		}
	}		
}

*/
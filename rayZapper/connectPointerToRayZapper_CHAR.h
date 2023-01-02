const char connectPointerToRayZapper[] =  " string $sel[] = `ls -sl`; " 
 " int $numSels = size($sel); " 
 " string $rayZapperNode = $sel[$numSels - 1]; " 
 " if( `nodeType $rayZapperNode` != \"rayZapperNode\") " 
 " { error( \"Please select transform nodes followed by one rayZapperNode.\"); } " 
 " int $i = 0; " 
 " for($i; $i < ($numSels - 1); $i++) " 
 " { " 
 " string $type = `nodeType $sel[$i]`; " 
 " if($type != \"transform\") " 
 " {  " 
 " error( \"Please select any number of transform nodes followed by one rayZapperNode.\");  " 
 " } " 
 " } " 
 " int $indeces[] = `getAttr -mi ($rayZapperNode + \".inTransform\")`; " 
 " int $size = size($indeces); " 
 " int $nextAvailableIndex = $size; " 
 " if(   ($size != 0) && ($indeces[$size - 1] > $size)   ) " 
 " { " 
 " $nextAvailableIndex = $indeces[$size - 1] + 1; " 
 " } " 
 " $i = 0; " 
 " for($i; $i < ($numSels - 1); $i++) " 
 " { " 
 " connectAttr ($sel[$i] + \".worldMatrix\") ($rayZapperNode + \".inTransform[\" + $nextAvailableIndex +\"].inMatrix\"); " 
 " getAttr ($rayZapperNode + \".inTransform[\" + $nextAvailableIndex + \" ].inPosition\"); " 
 " getAttr ($rayZapperNode + \".inTransform[\" + $nextAvailableIndex + \" ].direction\"); " 
 " $nextAvailableIndex++;  " 
 " } ";
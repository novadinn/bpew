
vec4 outlineSelectedMesh(vec4 origColor, vec4 selectionColor,
		     int currentEntityID, int selectedEntityID,
		     float mixFactor) {

    float mult = (sign(currentEntityID) + 1) * 0.5f;
    int hit = int(float(currentEntityID == selectedEntityID) * mult);
    
    return mix(origColor, selectionColor, mixFactor * hit);
}

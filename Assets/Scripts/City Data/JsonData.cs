using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JsonData
{
    public TypeMappingData type_Mapping;
    public List<UnitInfoData> fixed_unit;
    public List<UnitInfoData> free_unit;

    public UnitInfoData knob_time;
    public UnitInfoData knob_fixed_unit;
}

public class TypeMappingData
{
    public Fixed_UnitData fixed_unit;
    public Free_UnitData free_unit;
}

public class Fixed_UnitData
{
    public int livinglab;
    public int coworking;
    public int indoor;
    public int outdoor;
}

public class Free_UnitData
{
    public int publicBuilding;
    public int facilities;
}

public class UnitInfoData
{
    public int type;
    public float capacity;
    public float x;
    public float y;
    public float rot;
    public int fixed_unit_type;
    
}


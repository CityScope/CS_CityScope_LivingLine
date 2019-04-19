using System.Collections;
using System.Collections.Generic;
using UnityEngine;



public class JsonData
{
    public List<UnitInfoData> fixed_units;
    public List<UnitInfoData> free_units;
    public List<UnitInfoData> knobs;
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
    public float x;
    public float y;
    public float rot;
}

public class CoordinatesData
{
    public float[] x;
    public float[] y;
}

public class ID_type_capacity
{
    public float[] fixed_units;
    public float[] free_units;
}

public class AgentUnitIncrease
{
    public AgentUnitIncrease_Inner fixed_units;
    public AgentUnitIncrease_Inner free_units;
}

public class AgentUnitIncrease_Inner
{
    public float P1;
    public float P2;
    public float P3;
    public float P4;
}


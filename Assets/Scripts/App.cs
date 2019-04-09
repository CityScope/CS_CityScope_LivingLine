using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Newtonsoft.Json;

public class App : MonoBehaviour
{
    public UdpListener udpListener;
    public Vector3 offset;
    public FixedUnit[] fixedUnits;
    public GameObject gridUnitListRoot;
    private CoordinatesData data;

    void Awake()
    {
        TextAsset textAsset = Resources.Load<TextAsset>("JsonFile/CityScope_LivingLine_grid_cell_coordinates");
        data = JsonConvert.DeserializeObject<CoordinatesData>(textAsset.text);

    }


    void Start()
    {
        //fixedUnits = gridUnitListRoot.GetComponentsInChildren<FixedUnit>(true);
    }

    void OnGUI()
    {
        if (GUILayout.Button("UpdateData"))
        {
            UpdateData();
        }
    }

    void UpdateData()
    {
        TextAsset textAsset = Resources.Load<TextAsset>("JsonFile/Data_example");
        JsonData data = JsonConvert.DeserializeObject<JsonData>(textAsset.text);
        UpdateFixedUnit(data);
    }

    void UpdateFixedUnit(JsonData jsonData)
    {
        foreach (FixedUnit fixedUnit in this.fixedUnits)
        {
            fixedUnit.gameObject.SetActive(false);
        }

        foreach (FixedUnit fixedUnit in this.fixedUnits)
        {
            foreach (UnitInfoData infoData in jsonData.fixed_unit)
            {
                fixedUnit.Show(infoData,offset);
            }

        }
    }

}


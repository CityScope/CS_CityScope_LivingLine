using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Newtonsoft.Json;

public class App : MonoBehaviour
{
    public UdpListener udpListener;
    public Vector3 offset;
    public List<FixedUnit> fixedUnits;
    public GameObject gridUnitListRoot;
    private CoordinatesData data;

    private List<GameObject> pointList = new List<GameObject>();

    void Awake()
    {
        TextAsset textAsset = Resources.Load<TextAsset>("JsonFile/CityScope_LivingLine_grid_cell_coordinates");
        data = JsonConvert.DeserializeObject<CoordinatesData>(textAsset.text);
        GameObject pointRoot = GameObject.Find("PointRoot");
        if(pointRoot==null)
        {
            pointRoot = new GameObject("PointRoot");

        }
        for(int i=0;i< data.x.Length;i++)
        {
            GameObject go = new GameObject();
            go.transform.parent = pointRoot.transform;
            go.name = "Point." + i;
            go.transform.position = new Vector3(data.x[i]+5.5f, 152, data.y[i]);
            go.transform.localEulerAngles = new Vector3(0,90,0);
            go.transform.localScale = Vector3.one;
            pointList.Add(go);
            fixedUnits.Add(go.AddComponent<FixedUnit>());
            //GameObject grid = GameObject.Instantiate<GameObject>( Resources.Load<GameObject>("Community Center"));
            //grid.transform.parent = go.transform;
            //grid.transform.localPosition = Vector3.zero;
            //grid.transform.localEulerAngles = Vector3.zero;
            //grid.transform.localScale = Vector3.one;
        }
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
            fixedUnit.Show(null);
        }

        foreach (FixedUnit fixedUnit in this.fixedUnits)
        {
            foreach (UnitInfoData infoData in jsonData.fixed_unit)
            {
                if (fixedUnit.Show(infoData))
                    break;
            }

        }
    }

    public void CreateAI()
    {

    }

}


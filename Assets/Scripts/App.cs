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

    public bool useUDP = false;
    public string jsonData;

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

        // if use udp
        if (useUDP)
        {
            if (udpListener._encodedUDP == "")
            {
                Debug.LogWarning("UDP recieved null");
            }
            else
            {
                jsonData = udpListener._encodedUDP;
            }
        }

        // sample data
        // { "fixed_units": [{"type": 3, "x": 219.31, "y": 54.66}, ...
        // ... {"type": 6, "x": 113.88, "y": 93.92}], 
        // "free_units": [{"type": 8, "x": 185.51, "y": 91.03, "rot": 278.6}, ...
        // ... {"type": 8, "x": 9.38, "y": 103.15, "rot": 329.62}], 
        // "knobs": [{"type": 10, "x": 276.52, "y": 10.55, "rot": 274.2}, 
        // {"type": 11, "x": 290.22, "y": 106.35, "rot": 161.43}, 
        // {"type": 12, "x": 318.57, "y": 112.41, "rot": 122.77}]}
        
    }

    void OnGUI()
    {
        if (GUILayout.Button("UpdateData"))
        {
            UpdateData();
        }
    }

    void Update()
    {
        if (useUDP)
        {
            if (udpListener._encodedUDP == "")
            {
                Debug.LogWarning("UDP recieved null");
            }
            else
            {
                jsonData = udpListener._encodedUDP;

                UpdateData();
            }
        }
    }

    void UpdateData()
    {
        if (useUDP)
        {
            JsonData data = JsonConvert.DeserializeObject<JsonData>(jsonData);
            UpdateFixedUnit(data);
        }
        else
        {
            TextAsset textAsset = Resources.Load<TextAsset>("JsonFile/Data_example");
            JsonData data = JsonConvert.DeserializeObject<JsonData>(textAsset.text);
            UpdateFixedUnit(data);
        }
    }

    void UpdateFixedUnit(JsonData jsonData)
    {
        foreach (FixedUnit fixedUnit in this.fixedUnits)
        {
            fixedUnit.Show(null);
        }

        foreach (FixedUnit fixedUnit in this.fixedUnits)
        {
            foreach (UnitInfoData infoData in jsonData.fixed_units)
            {
                if (fixedUnit.Show(infoData))
                    break;
            }

        }
    }

    public void CreateAI()
    {

    }


    // for UI debug use udp
    public void ToggleUseUDP(bool toggleValue)
    {
        useUDP = toggleValue;
    }

}


using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Newtonsoft.Json;

public class App : MonoBehaviour
{
    #region typeMaping
    public static int Future_Education_Lab = 0;
    public static int Future_Food_Lab = 1;
    public static int Future_Art_Lab = 2;
    public static int Incubator = 3;
    public static int Coworking_Space = 4;
    public static int Market = 5;
    public static int Community_Center = 6;
    public static int Public_Space = 7;
    public static int Facility = 8;
    public static int Art_Installation = 9;
    public static int Time_Knob = 10;
    public static int Unit_Property_Knob = 11;
    public static int Viz_Type_Knob = 12;
    #endregion

    #region Data
    private CoordinatesData data;
    private ID_type_capacity IdCapacityProperty;
    private AgentUnitIncrease angentUnitIncrease;
    #endregion

    public UdpListener udpListener;
    public Vector3 offset;
    public List<FixedUnit> fixedUnits;
    
    public AstarPath astarPath;
    public Transform aiRoot;
    public GameObject pathPointRoot;
    public int maxAINum;
    public int perCreateAINum;

    #region PointList
    private List<GameObject> aiList = new List<GameObject>();
    private List<GameObject> unitPointList = new List<GameObject>();
    public List<PathPoint> allPathPointList = new List<PathPoint>();
    public List<PathPoint> workPathPointList = new List<PathPoint>();
    public List<PathPoint> residencePathPointList = new List<PathPoint>();
    public List<PathPoint> enterPathPointList = new List<PathPoint>();
    public List<PathPoint> exitPathPointList = new List<PathPoint>();
    #endregion

    // RZ
    public bool useUDP = true;
    public string jsonData;
    public List<GameObject> freeUnits;
    public GameObject freeUnitListRoot;
    public GameObject timeKnob;

    public const float c_UnitXoffset = 5.5f;

    void Awake()
    {
        LoadJson();

        GameObject pointRoot = GameObject.Find("UnitPointRoot");
        if(pointRoot==null)
        {
            pointRoot = new GameObject("UnitPointRoot");

        }
        for(int i=0;i< data.x.Length;i++)
        {
            GameObject go = new GameObject();
            go.transform.parent = pointRoot.transform;
            go.name = "Point." + i;
            go.transform.position = new Vector3(data.x[i]+ c_UnitXoffset, 152, data.y[i]);
            go.transform.localEulerAngles = new Vector3(0,90,0);
            go.transform.localScale = Vector3.one;
            unitPointList.Add(go);
            fixedUnits.Add(go.AddComponent<FixedUnit>());
        }

        allPathPointList = new List<PathPoint>(this.pathPointRoot.GetComponentsInChildren<PathPoint>());
        workPathPointList = allPathPointList.FindAll(point => point.pathPointType == PathPoint.PathPointType.Work);
        residencePathPointList = allPathPointList.FindAll(point => point.pathPointType == PathPoint.PathPointType.Residence);
        enterPathPointList = allPathPointList.FindAll(point => point.pathPointType == PathPoint.PathPointType.Enter);
        exitPathPointList = allPathPointList.FindAll(point => point.pathPointType == PathPoint.PathPointType.Exit);
        
    }

    void Start()
    {
        Invoke("CreateAITimer", 2);
        InvokeRepeating("CreateAITimer", 20,20);

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

    void LoadJson()
    {
        TextAsset textAsset = Resources.Load<TextAsset>("JsonFile/CityScope_LivingLine_grid_cell_coordinates");
        data = JsonConvert.DeserializeObject<CoordinatesData>(textAsset.text);

        textAsset = Resources.Load<TextAsset>("JsonFile/CityScope_LivingLine_type_capacity");
        IdCapacityProperty = JsonConvert.DeserializeObject<ID_type_capacity>(textAsset.text);

        textAsset = Resources.Load<TextAsset>("JsonFile/CityScope_LivingLine_agent type_unit increase");
        angentUnitIncrease = JsonConvert.DeserializeObject<AgentUnitIncrease>(textAsset.text);
    }

    void CreateAITimer()
    {
        if(aiList.Count< this.maxAINum)
        {
            int randomNum = Random.Range(1,this.perCreateAINum+1);
            for (int i = 0; i < this.perCreateAINum; i++)
            {
                CreateAI();
            }
        }
    }

    void CreateAllAI()
    {
        //fixedUnits = gridUnitListRoot.GetComponentsInChildren<FixedUnit>(true);
        for (int i = 0; i < maxAINum; i++)
        {
            CreateAI();
        }
    }
    public void CreateAI()
    {
        int bornNum = Random.Range(1, 6);
        GameObject bornPoint = Tool.GetChildInDepth("BornPoint" + bornNum, this.aiRoot.gameObject);
        CreateAI(bornPoint.transform.position);
    }

    public void CreateAI(Vector3 pos)
    {
        int num = Random.Range(0, 100);
        int index = 0;
        if (num <= 50)
        {
            index = 1;
        }
        else if (num <= 65)
        {
            index = 2;
        }
        else if (num <= 95)
        {
            index = 3;
        }
        else
        {
            index = 4;
        }
        GameObject aiGo = GameObject.Instantiate<GameObject>(Resources.Load<GameObject>("Characters/AI_P" + index));
        aiGo.transform.position = astarPath.GetNearest(pos).position;
        aiList.Add(aiGo);
    }

    public void RemoveAI(GameObject aiGo)
    {
        aiList.Remove(aiGo);
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
        if (useUDP)
        {
            JsonData data = JsonConvert.DeserializeObject<JsonData>(jsonData);
            UpdateFixedUnits(data);
            UpdateFreeUnits(data);
            UpdateKnobs(data);
        }
        else
        {
            TextAsset textAsset = Resources.Load<TextAsset>("JsonFile/Data_example");
            JsonData data = JsonConvert.DeserializeObject<JsonData>(textAsset.text);
            UpdateFixedUnits(data);
            UpdateFreeUnits(data);
            UpdateKnobs(data);
        }
    }

    void UpdateFixedUnits(JsonData jsonData)
    {
        foreach (FixedUnit fixedUnit in this.fixedUnits)
        {
            fixedUnit.Show(null);
        }

        foreach (FixedUnit fixedUnit in this.fixedUnits)
        {
            bool bShowUnit = false;
            foreach (UnitInfoData infoData in jsonData.fixed_units)
            {
                
                if (fixedUnit.Show(infoData))
                {
                    bShowUnit = true;
                    break;
                }
            }

            if(bShowUnit)
            {

            }
        }
    }

    // RZ TODO: need to be optimized, now deleting and re-instantiate every frame
    void UpdateFreeUnits(JsonData jsonData)
    {
        // clean up last frame GOs
        if (freeUnits.Count > 0)
        {
            foreach (GameObject go in freeUnits)
            {
                Destroy(go);
            }
        }
        freeUnits = new List<GameObject>();

        // add new GOs according to json via udp
        foreach (UnitInfoData infoData in jsonData.free_units)
        {
            string sourceName = null;
            switch (infoData.type)
            {
                case 7:
                    sourceName = "Public Space";
                    break;
                case 8:
                    sourceName = "Facility";
                    break;
                case 9:
                    sourceName = "Art Installation";
                    break;
            }
            GameObject go = Instantiate(Resources.Load<GameObject>(sourceName));
            go.transform.parent = freeUnitListRoot.transform;
            go.transform.localPosition = new Vector3(infoData.x, 0.0f, infoData.y);
            go.transform.localEulerAngles = new Vector3(0.0f, -infoData.rot, 0.0f);
            go.transform.localScale = Vector3.one;
            freeUnits.Add(go);
        }
    }

    void UpdateKnobs(JsonData jsonData)
    {
        // time knob
        foreach (UnitInfoData infoData in jsonData.knobs)
        {
            if (infoData.type == 10)
            {
                timeKnob.transform.localPosition = new Vector3(infoData.x, 0.0f, infoData.y);
                timeKnob.GetComponent<TimeKnob>().knobValue = 1.0f - infoData.rot / 360.0f;
            }
        }
    }

    public PathPoint GetRandomPoint(PathPoint.PathPointType pointType)
    {
        List<PathPoint> pointList = this.workPathPointList;

        if(pointType == PathPoint.PathPointType.Work)
        {
            pointList = this.workPathPointList;
        }
        else if(pointType == PathPoint.PathPointType.Residence)
        {
            pointList = this.residencePathPointList;
        }
        else if(pointType == PathPoint.PathPointType.Enter)
        {
            pointList = this.enterPathPointList;
        }
        else if(pointType == PathPoint.PathPointType.Exit)
        {
            pointList = this.exitPathPointList;
        }

        PathPoint point = pointList[Random.Range(0, pointList.Count)];
        //Debug.Log("GetRandomPoint...."+point.gameObject.name);
        return point;
    }


    // for UI debug use udp
    public void ToggleUseUDP(bool toggleValue)
    {
        useUDP = toggleValue;
    }
}


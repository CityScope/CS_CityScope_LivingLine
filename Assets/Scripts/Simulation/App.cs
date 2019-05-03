using System.Collections;
using System.Collections.Generic;
using UnityEngine;

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
    public float createAITimer = 100;
    public int addUnitCreateAINum = 10;

    public float influenceAIDistance;
    public float influenceAINum;

    #region PointList
    private List<GameObject> aiList = new List<GameObject>();
    private List<GameObject> unitPointList = new List<GameObject>();
    public List<PathPoint> allPathPointList = new List<PathPoint>();
    public List<PathPoint> workPathPointList = new List<PathPoint>();
    public List<PathPoint> residencePathPointList = new List<PathPoint>();
    public List<PathPoint> enterPathPointList = new List<PathPoint>();
    public List<PathPoint> exitPathPointList = new List<PathPoint>();
    #endregion

    public const float c_UnitXoffset = 0f;  //5.5f;

    // RZ
    public bool useUDP = true;
    public string jsonData;
    public List<GameObject> freeUnits;
    public GameObject freeUnitListRoot;
    public GameObject timeKnob;

    public bool remap = false;
    public float xRemapMin = 320f;
    public float xRemapMax = 220f;
    public float yRemapMin = 140f;
    public float yRemapMax = 0f;
    public float camResW = 1280f;
    public float camResH = 720f;

    public bool moveKnobsToSidePanel = false;
    public float xOffsetSidePanel = -487.2f;
    public float yOffsetSidePanel = -152f;
    public float zOffsetSidePanel = -70f;

    public BoxMorphKeyPts boxMorphKeyPtsMain;

    private Dictionary<string, GameObject> aiPrefabDic = new Dictionary<string, GameObject>();
    
    private float timeSinceLastCalled;
    public float gcDelay = 1f;


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
            // RZ TODO: might have memory leak
            FixedUnit goFixedUnit = go.AddComponent<FixedUnit>();
            goFixedUnit.boxMorphKeyPtsMain = boxMorphKeyPtsMain;
            unitPointList.Add(go);
            fixedUnits.Add(goFixedUnit);
            //fixedUnits.Add(go.AddComponent<FixedUnit>());
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
        InvokeRepeating("CreateAITimer", createAITimer, createAITimer);

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


        // initiate free units
        InitiateFreeUnits();

    }

    void Update()
    {
        if (useUDP)
        {
            if (udpListener._encodedUDP == "")
            {
                //Debug.LogWarning("UDP recieved null");
            }
            else
            {
                jsonData = udpListener._encodedUDP;

                UpdateData();
            }
        }
        
        /*
        // periodic GC
        timeSinceLastCalled += Time.deltaTime;
        if (timeSinceLastCalled > gcDelay)
        {
            System.GC.Collect();
            Resources.UnloadUnusedAssets();
            timeSinceLastCalled = 0f;
        }
        */
    }

    void LoadJson()
    {
        TextAsset textAsset = Resources.Load<TextAsset>("JsonFile/CityScope_LivingLine_grid_cell_coordinates");
        data = Newtonsoft.Json.JsonConvert.DeserializeObject<CoordinatesData>(textAsset.text);
        
        textAsset = Resources.Load<TextAsset>("JsonFile/CityScope_LivingLine_type_capacity");
        IdCapacityProperty = Newtonsoft.Json.JsonConvert.DeserializeObject<ID_type_capacity>(textAsset.text);

        textAsset = Resources.Load<TextAsset>("JsonFile/CityScope_LivingLine_agent type_unit increase");
        angentUnitIncrease = Newtonsoft.Json.JsonConvert.DeserializeObject<AgentUnitIncrease>(textAsset.text);
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
        string aiPath = "Characters/AI_P" + index;
        GameObject prefab = null;
        if(aiPrefabDic.ContainsKey(aiPath))
        {
            prefab = aiPrefabDic[aiPath];
        }else
        {
            prefab = Resources.Load<GameObject>(aiPath);
            aiPrefabDic[aiPath] = prefab;
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
            JsonData data = Newtonsoft.Json.JsonConvert.DeserializeObject<JsonData>(jsonData);
            UpdateFixedUnits(data);
            UpdateFreeUnits(data);
            UpdateKnobs(data);
        }
        else
        {
            TextAsset textAsset = Resources.Load<TextAsset>("JsonFile/CityScope_LivingLine_input_data");
            JsonData data = Newtonsoft.Json.JsonConvert.DeserializeObject<JsonData>(textAsset.text); 
            //JsonData data = JsonUtility.FromJson<JsonData>(textAsset.text);
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
        }
    }

    Dictionary<int, GameObject> freeUnitDic = new Dictionary<int, GameObject>(); 

    void InitiateFreeUnits()
    {
        // RZ TODO: update to support 9 free units with 9 unique aruco tags
        // initiate 3(to be updated to 9) GO for 3 type of free units
        for (int i = 7; i <= 9; i++)
        {
            string sourceName = null;
            switch (i)
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
            go.transform.localPosition = new Vector3(0.0f, 0.0f, 0.0f);
            go.transform.localEulerAngles = new Vector3(0.0f, 0.0f, 0.0f);
            go.transform.localScale = Vector3.one;
            // RZ TODO: might have memory leak
            // follow main box morph
            go.GetComponent<ManuallyAddBoxMorphGO>().boxMorphKeyPts = boxMorphKeyPtsMain;
            go.GetComponent<ManuallyAddBoxMorphGO>().alwaysUpdateMorph = true;
            go.SetActive(false);
            freeUnits.Add(go);
        }
    }
    
    void UpdateFreeUnits(JsonData jsonData)
    {
        // RZ TODO: update to support 9 free units with 9 unique aruco tags
        // switch off all free units first
        foreach (GameObject go in freeUnits)
        {
            go.SetActive(false);
        }

        // switch on the free units according to udp data and update position
        foreach (UnitInfoData infoData in jsonData.free_units)
        {
            GameObject go = null;
            switch (infoData.type)
            {
                case 7:
                    go = freeUnits[0];
                    break;
                case 8:
                    go = freeUnits[1];
                    break;
                case 9:
                    go = freeUnits[2];
                    break;
            }
            go.SetActive(true);
            // update position
            if (remap)
            {
                go.transform.localPosition = new Vector3(Remap(infoData.x, 0f, camResW, xRemapMin, xRemapMax), 0.0f, Remap(infoData.y, 0f, camResH, yRemapMin, yRemapMax));
            }
            else
            {
                go.transform.localPosition = new Vector3(infoData.x, 0.0f, infoData.y);
            }
            go.transform.localEulerAngles = new Vector3(0.0f, -infoData.rot, 0.0f);
            go.transform.localScale = Vector3.one;
        }
    }

    void UpdateKnobs(JsonData jsonData)
    {
        // time knob
        foreach (UnitInfoData infoData in jsonData.knobs)
        {
            if (infoData.type == 10)
            {
                float dx = 0f;
                float dy = 0f;
                float dz = 0f;
                if (moveKnobsToSidePanel)
                {
                    dx = xOffsetSidePanel;
                    dy = yOffsetSidePanel;
                    dz = zOffsetSidePanel;
                }
                if (remap)
                {
                    timeKnob.transform.localPosition = new Vector3(Remap(infoData.x, 0f, camResW, xRemapMin, xRemapMax) + dx, dy, Remap(infoData.y, 0f, camResH, yRemapMin, yRemapMax) + dz);
                }
                else
                {
                    timeKnob.transform.localPosition = new Vector3(infoData.x + dx, dy, infoData.y + dz);
                }
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

    float Remap(float from, float fromMin, float fromMax, float toMin, float toMax)
    {
        var fromAbs = from - fromMin;
        var fromMaxAbs = fromMax - fromMin;

        var normal = fromAbs / fromMaxAbs;

        var toMaxAbs = toMax - toMin;
        var toAbs = toMaxAbs * normal;

        var to = toAbs + toMin;

        return to;
    }

    public List<GameObject> GetNearListAI(Vector3 pos)
    {
        List<GameObject> goList = new List<GameObject>();

        foreach(GameObject go in this.aiList)
        {
            if(Vector3.Distance( go.transform.position,pos)<= influenceAIDistance&&goList.Count<influenceAINum)
            {
                goList.Add(go);
            }
        }

        return goList;
    }
}


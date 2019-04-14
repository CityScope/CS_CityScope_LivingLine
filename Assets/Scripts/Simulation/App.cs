using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Newtonsoft.Json;

public class App : MonoBehaviour
{
    
    public UdpListener udpListener;
    public Vector3 offset;
    public List<FixedUnit> fixedUnits;
    private CoordinatesData data;
    public AstarPath astarPath;
    public Transform aiRoot;
    public GameObject pathPointRoot;

    public int initNum;


    private List<GameObject> pointList = new List<GameObject>();

    public List<GameObject> pathTargetPointList = new List<GameObject>();

    public List<PathPoint> workPathPointList = new List<PathPoint>();
    public List<PathPoint> residencePathPointList = new List<PathPoint>();

    void Awake()
    {
        TextAsset textAsset = Resources.Load<TextAsset>("JsonFile/CityScope_LivingLine_grid_cell_coordinates");
        data = JsonConvert.DeserializeObject<CoordinatesData>(textAsset.text);
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
            go.transform.position = new Vector3(data.x[i]+5.5f, 152, data.y[i]);
            go.transform.localEulerAngles = new Vector3(0,90,0);
            go.transform.localScale = Vector3.one;
            pointList.Add(go);
            fixedUnits.Add(go.AddComponent<FixedUnit>());
        }

        List<PathPoint>  pathPointList = new List<PathPoint>(this.pathPointRoot.GetComponentsInChildren<PathPoint>());
        workPathPointList = pathPointList.FindAll(point => point.pathPointType == PathPoint.PathPointType.Work);
        residencePathPointList = pathPointList.FindAll(point => point.pathPointType == PathPoint.PathPointType.Residence);
    }


    void Start()
    {
        Invoke("CreateAllAI", 2);
       
       
    }


    void CreateAllAI()
    {
        //fixedUnits = gridUnitListRoot.GetComponentsInChildren<FixedUnit>(true);
        for (int i = 0; i < initNum; i++)
        {
            CreateAI();
        }
    }
    public void CreateAI()
    {
        GameObject aiGo = GameObject.Instantiate<GameObject>(Resources.Load<GameObject>("Characters/AI"));
        GameObject bornPoint = Tool.GetChildInDepth("BornPoint1", this.aiRoot.gameObject);
        aiGo.transform.position =  astarPath.GetNearest(bornPoint.transform.position).position;

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

    public PathPoint GetRandomPoint(bool isWorkPoint)
    {
        List<PathPoint> pointList = this.workPathPointList;


        if(!isWorkPoint)
        {
            pointList = this.residencePathPointList;
        }

        PathPoint point = pointList[Random.Range(0, pointList.Count)];
        //Debug.Log("GetRandomPoint...."+point.gameObject.name);
        return point;
    }


}


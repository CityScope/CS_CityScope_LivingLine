using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine.EventSystems;

/// <summary>
/// 输入数据
/// </summary>
public class AYInput
{
    public enum AYInputState
    {
        start,
        move,
        stay,
        leave,
        mouseDown
    }

    public Vector2 screenInputPos;
    public Vector3 groundInputPos;
    public AYInputState state;
    public RaycastHit[] hits;
    public Vector2 screenDelta;
    public Vector2 screenTotalDelta;

    public Vector3 delta;
    public Vector3 totalDelta;


    public GameObject FirstHitGameObject
    {
        get
        {
            if (hits == null)
            {
                return null;
            }

            if (hits.Length == 0) return null;
            return hits[0].collider.gameObject;
        }
    }

    public Vector3 FirstHitPos
    {
        get
        {
            if (hits.Length == 0) return Vector3.zero;
            return hits[0].point;
        }
    }
}

/// <summary>
/// 输入控制类
/// </summary>
public class AYTouch : MonoBehaviour
{
    private static AYTouch instance;
    //public Touch lastTouch;


    public static bool useTouch = true;

    public static Vector3 screenOffset0;
    public static Vector3 screenOffsetx;
    public static Vector3 screenOffsety;
    public static Vector3 screenOffsetxy;
    public static Vector3 screenOffsetCenter;

    public static Vector3 CenterOffset;

    public AYInput input;

    private List<AYInput> inputList = new List<AYInput>();


    public LayerMask layerMask;

    void Awake()
    {
        instance = this;
    }

    void Start()
    {
        Time.timeScale = 1f;
    }
    public static AYTouch getInstance()
    {
        return instance;
    }


    // Update is called once per frame
    void Update()
    {
#if IPHONE || ANDROID
		if (EventSystem.current.IsPointerOverGameObject(Input.GetTouch(0).fingerId))
#else
        if (EventSystem.current.IsPointerOverGameObject())
#endif
            return;

        getTouchPlanPossition();


        getAYInput();

    }

    public Vector3 getScrrenCenter()
    {
        AYInput input = new AYInput();
        input.screenInputPos = new Vector2(Screen.width / 2, Screen.height / 2);
        raycastHit(input);
        screenOffsetCenter = input.groundInputPos;
        return screenOffsetCenter;
    }

    void getAYInput()
    {
        this.input = null;
        AYInput ayInput = getMouseInput();
        if (ayInput == null) return;

        if (ayInput.state == AYInput.AYInputState.start && inputList.Count > 0)
        {
            inputList.Clear();
        }

        raycastHit(ayInput);
        inputList.Add(ayInput);


       

        if (inputList.Count > 1)
        {
            ayInput.screenDelta = inputList[inputList.Count - 1].screenInputPos - inputList[inputList.Count - 2].screenInputPos;
            ayInput.screenTotalDelta = inputList[inputList.Count - 1].screenInputPos - inputList[0].screenInputPos;
        }

        
        if ((ayInput.state == AYInput.AYInputState.move || ayInput.state == AYInput.AYInputState.mouseDown) && !ayInput.screenTotalDelta.Equals(Vector2.zero)
                   && ayInput.screenDelta.magnitude > 0)
        {
            //App.Instance.ScreenInput(ayInput.screenDelta, ayInput.totalDelta);
        }

        if (inputList[0].FirstHitGameObject == null)
        {
            return;
        }

        this.input = ayInput;
        if (inputList.Count > 1)
        {
            ayInput.delta = inputList[inputList.Count - 1].FirstHitPos - inputList[inputList.Count - 2].FirstHitPos;
            ayInput.totalDelta = inputList[inputList.Count - 1].FirstHitPos - inputList[0].FirstHitPos;
            ayInput.screenDelta = inputList[inputList.Count - 1].screenInputPos - inputList[inputList.Count - 2].screenInputPos;
            ayInput.screenTotalDelta = inputList[inputList.Count - 1].screenInputPos - inputList[0].screenInputPos;
        }

        Component[] components = inputList[0].FirstHitGameObject.GetComponents<Component>();
        foreach (Component c in components)
        {
            if (c == null) continue;
            if (typeof(IInputListener).IsAssignableFrom(c.GetType()))
            {
                IInputListener listener = (IInputListener)c;
                listener.updateInputs(inputList);

                if ((ayInput.state == AYInput.AYInputState.move || ayInput.state == AYInput.AYInputState.mouseDown) && !ayInput.screenTotalDelta.Equals(Vector2.zero)
                    && ayInput.screenDelta.magnitude > 0)
                {
                    listener.inputScreenMove(ayInput.screenDelta, ayInput.screenTotalDelta, ayInput);
                    listener.inputWorldMove(ayInput.delta, ayInput.totalDelta, ayInput);
                }

                if (ayInput.state == AYInput.AYInputState.start)
                {
                    listener.OnPressed(true);
                }
                else if (ayInput.state == AYInput.AYInputState.leave)
                {
                    listener.OnPressed(false);
                }
            }
        }


        if (ayInput.state == AYInput.AYInputState.leave)
        {
            // Debug.LogError("inputList[0].FirstHitGameObject...." + inputList[0].FirstHitGameObject.gameObject.name);
            //inputList[0].FirstHitGameObject.SendMessage("OnClick");
        }
    }


    AYInput getMouseInput()
    {
        AYInput ayInput = null;
        if (Input.GetMouseButtonDown(0))
        {
            ayInput = new AYInput();
            ayInput.state = AYInput.AYInputState.start;
        }
        if (Input.GetMouseButtonUp(0))
        {
            ayInput = new AYInput();
            ayInput.state = AYInput.AYInputState.leave;
        }
        if (Input.GetMouseButton(0) && !Input.GetMouseButtonDown(0))
        {
            ayInput = new AYInput();
            ayInput.state = AYInput.AYInputState.mouseDown;
        }

        if (ayInput != null)
        {
            ayInput.screenInputPos = Input.mousePosition;
            return ayInput;
        }
        else
        {
            return null;
        }
    }

    AYInput getTouchInput()
    {
        AYInput ayInput = null;

        if (Input.touchCount == 0)
        {
            return null;
        }


        Touch touch = Input.GetTouch(0);

        if (touch.phase == TouchPhase.Began)
        {
            ayInput = new AYInput();
            ayInput.state = AYInput.AYInputState.start;
        }

        if (touch.phase == TouchPhase.Canceled || touch.phase == TouchPhase.Ended)
        {
            ayInput = new AYInput();
            ayInput.state = AYInput.AYInputState.leave;
        }

        if (touch.phase == TouchPhase.Moved)
        {
            ayInput = new AYInput();
            ayInput.state = AYInput.AYInputState.move;
        }

        if (touch.phase == TouchPhase.Stationary)
        {
            ayInput = new AYInput();
            ayInput.state = AYInput.AYInputState.stay;
        }

        if (ayInput != null)
        {
            ayInput.screenInputPos = touch.position;
        }
        return ayInput;
    }


    void raycastHit(AYInput input)
    {
        Camera camera = this.GetComponent<Camera>();

        Ray ray = camera.ScreenPointToRay(input.screenInputPos);
        Debug.DrawRay(ray.origin, ray.direction * 100, Color.yellow);
        float dist = camera.farClipPlane - camera.nearClipPlane;
        RaycastHit[] hits = Physics.RaycastAll(ray, dist);
        System.Array.Sort(hits, delegate (RaycastHit r1, RaycastHit r2) { return r1.distance.CompareTo(r2.distance); });
        foreach (RaycastHit hit in hits)
        {
            /**Ground g = hit.collider.GetComponent<Ground>();

            if(g!=null)
            {
                print("!!!!!");
                input.groundInputPos = hit.point;
            }else
            {
                GroundChild gc = hit.collider.GetComponent<GroundChild>();
                if(gc!=null)
                {
                    input.groundInputPos = hit.point;
                }
            }**/
        }
        input.hits = hits;
    }

    public void getTouchPlanPossition()
    {
        Vector2 position = Vector2.zero;
        Camera camera = this.GetComponent<Camera>();
        if (Input.touchCount == 0)
        {

            if (Input.GetMouseButton(0))
            {
                position = Input.mousePosition;

                //if(Input.GetMouseButtonDown(0)) 
            }
            else
            {
                return;
            }
        }
        else
        {
            Touch input = Input.GetTouch(0);
            position = input.position;
        }

        Input.GetMouseButtonDown(0);


        Ray ray = camera.ScreenPointToRay(position);
        float dist = camera.farClipPlane - camera.nearClipPlane;
        RaycastHit[] hits = Physics.RaycastAll(ray, dist);
        System.Array.Sort(hits, delegate (RaycastHit r1, RaycastHit r2) { return r1.distance.CompareTo(r2.distance); });
        foreach (RaycastHit hit in hits)
        {
            /**GroundChild g = hit.collider.GetComponent<GroundChild>();
            if(g!=null)
            {
                inputPos =  hit.point;
                //g.hitGround(hit);
                
            }**/
        }
    }
}

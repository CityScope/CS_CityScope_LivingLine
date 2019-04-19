using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Tool
{
    public static GameObject GetChildInDepth(string name, GameObject rootGO)
    {
        Transform transform = rootGO.transform;
        for (int i = 0; i < transform.childCount; i++)
        {
            if (transform.GetChild(i).gameObject.name == name)
            {
                return transform.GetChild(i).gameObject;
            }
        }

        for (int i = 0; i < transform.childCount; i++)
        {
            GameObject go = GetChildInDepth(name, transform.GetChild(i).gameObject);
            if (go != null)
            {
                return go;
            }
        }
        return null;
    }

    public static T GetChildInDepth<T>(string name, Transform transform) where T : Component
    {
        for (int i = 0; i < transform.childCount; i++)
        {
            if (transform.GetChild(i).gameObject.name == name)
            {
                T t = transform.GetChild(i).GetComponent<T>();
                if (t != null)
                {
                    return t;
                }

            }
        }

        for (int i = 0; i < transform.childCount; i++)
        {
            T tDepth = GetChildInDepth<T>(name, transform.GetChild(i));
            if (tDepth != null)
            {
                return tDepth;
            }
        }

        return null;
    }
}

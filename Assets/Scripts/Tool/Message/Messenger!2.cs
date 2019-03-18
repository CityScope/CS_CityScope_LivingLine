using System;
using System.Collections.Generic;

namespace DMessager
{
	public static class Messenger<M, T>
	{
	    private static Dictionary<M, Delegate> eventTable;
	
	    static Messenger()
	    {
	        Messenger<M, T>.eventTable = MessengerInternal<M>.eventTable;
	    }
	
	    public static void addListener(M messageType, OnMessageDelegate<M, T> handler)
	    {
	        MessengerInternal<M>.onAddListener(messageType, handler);
	        Messenger<M, T>.eventTable[messageType] = (OnMessageDelegate<M, T>) Delegate.Combine((OnMessageDelegate<M, T>) Messenger<M, T>.eventTable[messageType], handler);
	    }
	
	    public static void postMessage(M messageType, T arg1)
	    {
	        Delegate delegate2;
	        if (Messenger<M, T>.eventTable.TryGetValue(messageType, out delegate2))
	        {
	            OnMessageDelegate<M, T> delegate3 = delegate2 as OnMessageDelegate<M, T>;
	            if (delegate3 != null)
	            {
	                delegate3(arg1);
	            }
	        }
	    }
	
	    public static void removeListener(M messageType, OnMessageDelegate<M, T> handler)
	    {
	        if (Messenger<M, T>.eventTable.ContainsKey(messageType))
	        {
	            Messenger<M, T>.eventTable[messageType] = (OnMessageDelegate<M, T>) Delegate.Remove((OnMessageDelegate<M, T>) Messenger<M, T>.eventTable[messageType], handler);
	            MessengerInternal<M>.onRemoveListener(messageType);
	        }
	    }
	}
}


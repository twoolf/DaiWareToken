// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Controls;

namespace MPinSDK.Common
{
    public static class Extensions
    {
        private static object Data;

        /// <summary>
        /// Causes the Frame to load content represented by the specified Page, also
        /// passing a data to be interpreted by the target of the navigation.        ///  
        /// </summary>
        /// <param name="frame">The frame itself.</param>
        /// <param name="sourcePageType">The URI of the content to navigate to.</param>
        /// <param name="data">The data that you need to pass to the other page 
        /// specified in URI.</param>
        public static bool Navigate(this Frame frame, Type sourcePageType, object data)
        {
            Data = data;
            return frame.Navigate(sourcePageType);
        }

        /// <summary>
        /// Navigates to the most recent item in back navigation history, if a Frame
        ///  manages its own navigation history.
        /// </summary>
        /// <param name="frame">The frame itself.</param>
        /// <param name="data">The data that you need to pass to the other page 
        /// specified in URI.</param>
        public static void GoBack(this Frame frame, object data)
        {
            Data = data;
            frame.GoBack();
        }

        /// <summary>
        /// Gets the navigation data passed from the previous page.
        /// </summary>
        /// <param name="service">The service.</param>
        /// <returns>System.Object.</returns>
        public static object GetNavigationData(this Frame service)
        {
            return Data;
        }
    }
}

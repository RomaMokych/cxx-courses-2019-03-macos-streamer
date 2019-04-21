//
//  ModelController.swift
//  Controll_IOS_Project
//
//  Created by MacOS on 18.04.2019.
//  Copyright © 2019 MacOS. All rights reserved.
//

import Foundation

class ModelData
{
    var receiver : MessageReceiver?
    var systemAlert: String = ""

    init(mainViewController : MainViewController)
    {
        receiver = MessageReceiver(mainViewController: mainViewController)
    }
}

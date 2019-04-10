//
//  MainViewController.swift
//  Controll_IOS_Project
//
//  Created by MacOS on 02.04.2019.
//  Copyright © 2019 MacOS. All rights reserved.
//

import UIKit

class MainViewController: UIViewController, UITextFieldDelegate {

    
    @IBOutlet weak var systemAlert: UITextView!
    @IBOutlet weak var ipTextField: UITextField!
    @IBAction func connect(_ sender: UIButton) {
        if checkIp(ip: ipTextField.text!){
            let vc = storyboard?.instantiateViewController(withIdentifier: "viewStream")
            self.present(vc!, animated: false, completion: nil)
            MessageReceiver.ip = ipTextField.text!
            print("connect")
        } else {
            systemAlert.textColor = UIColor.red
            systemAlert.text = "Wrong ip"
            print("connect failed, error ip")
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        ipTextField.delegate = self
        
        //Listen keyboards events
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardChange(notification:)), name: UIResponder.keyboardWillShowNotification , object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardChange(notification:)), name: UIResponder.keyboardWillHideNotification, object: nil)
    }
    
    @objc func textFieldMy(nc : NSNotification){
        
    }
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        self.view.endEditing(true)   //remove focus keyboard
        return true
    }
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        self.view.endEditing(true)   //remove focus keyboard
    }
    func checkIp(ip : String) -> Bool{
        let pattern = "(25[0-5]|2[0-4]\\d|1\\d{2}|\\d{1,2})\\.(25[0-5]|2[0-4]\\d|1\\d{2}|\\d{1,2})\\.(25[0-5]|2[0-4]\\d|1\\d{2}|\\d{1,2})\\.(25[0-5]|2[0-4]\\d|1\\d{2}|\\d{1,2})"
        let regexText = NSPredicate(format: "SELF MATCHES %@", pattern)
        let result = regexText.evaluate(with: ip)
        return result
    }
    
    //Mark: - Notification
    //Change frame view when call keyboard
    @objc func keyboardChange(notification: Notification){
        guard let keyboardSize = (notification.userInfo?[UIResponder.keyboardFrameEndUserInfoKey] as? NSValue)?.cgRectValue
            else { return }
        if notification.name == UIResponder.keyboardWillShowNotification{
            view.frame.origin.y = -keyboardSize.height
        } else if notification.name == UIResponder.keyboardWillHideNotification{
            view.frame.origin.y = 0
        }
    }
 
    deinit {
        NotificationCenter.default.removeObserver(UIResponder.keyboardWillShowNotification)
        NotificationCenter.default.removeObserver(UIResponder.keyboardWillHideNotification)
    }

}

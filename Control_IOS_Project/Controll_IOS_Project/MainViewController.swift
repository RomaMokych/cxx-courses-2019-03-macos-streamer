//
//  MainViewController.swift
//  Controll_IOS_Project
//
//  Created by MacOS on 02.04.2019.
//  Copyright © 2019 MacOS. All rights reserved.
//


import UIKit

class MainViewController: UIViewController, UITextFieldDelegate {
    var modelData: ModelData!
    @IBOutlet weak var systemAlert: UITextView!
    @IBOutlet weak var ipTextField: UITextField!
    @IBOutlet var buttonConnect: UIButton!
    @IBOutlet weak var activityIndecator: UIActivityIndicatorView!
    @IBAction func connect(_ sender: UIButton) {
        
        if checkIp(ip: ipTextField.text!)
        {
            print("Connecting to server...")
            activityIndecator.startAnimating()
            buttonConnect.isEnabled = true;
            modelData.receiver.setupNetworkCommunication(ipAddress: ipTextField.text!)
        } else {
            systemAlert.textColor = UIColor.red
            systemAlert.text = "Invalid IP address!"
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        ipTextField.delegate = self
        
        //Listen keyboards events
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardChange(notification:)), name: UIResponder.keyboardWillShowNotification , object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardChange(notification:)), name: UIResponder.keyboardWillHideNotification, object: nil)
        //Listen connection status
        NotificationCenter.default.addObserver(self, selector: #selector(connect(notification:)), name: MessageReceiver.connectionNotification, object: nil)
    }
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
    }
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if let VC = segue.destination as? ViewController{
            VC.modelData = modelData
        }
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
            view.frame.origin.y = -keyboardSize.height/2
        } else if notification.name == UIResponder.keyboardWillHideNotification{
            view.frame.origin.y = 0
        }
    }
    @objc func connect(notification: Notification){
        if let connectStatus = (notification.userInfo?[MessageReceiver.connectionStatusInfoKey] as? Bool){
            
            if connectStatus {
                print("Successfuly connected!")
                systemAlert.text = ""
                performSegue(withIdentifier: "goStreamViewController", sender: self)
            } else {
                if let connectMsg = (notification.userInfo?[MessageReceiver.messageAlertInfoKey] as? String){
                    systemAlert.textColor = UIColor.red
                    systemAlert.text = connectMsg
                }
            }
            activityIndecator.stopAnimating()
            buttonConnect.isEnabled = true;
        }
    }
    deinit {
        NotificationCenter.default.removeObserver(UIResponder.keyboardWillShowNotification)
        NotificationCenter.default.removeObserver(UIResponder.keyboardWillHideNotification)
        NotificationCenter.default.removeObserver(MessageReceiver.connectionNotification)
    }

}


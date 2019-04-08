//
//  MainViewController.swift
//  Controll_IOS_Project
//
//  Created by MacOS on 02.04.2019.
//  Copyright © 2019 MacOS. All rights reserved.
//

import UIKit

class MainViewController: UIViewController, UITextFieldDelegate {

    
    @IBOutlet weak var ipTextField: UITextField!
    @IBAction func connect(_ sender: UIButton) {
        if checkIp(ip: ipTextField.text!){
            let vc = storyboard?.instantiateViewController(withIdentifier: "viewStream")
            self.present(vc!, animated: false, completion: nil)
            MessageReceiver.ip = ipTextField.text!
            print("connect")
        } else {
            print("connect failed, error ip")
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        ipTextField.delegate = self
        
        // Do any additional setup after loading the view.
    }
    
    @objc func textFieldMy(nc : NSNotification){
        
    }
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        print(ipTextField.text)
        return true
    }
    func checkIp(ip : String) -> Bool{
        let pattern = "(25[0-5]|2[0-4]\\d|1\\d{2}|\\d{1,2})\\.(25[0-5]|2[0-4]\\d|1\\d{2}|\\d{1,2})\\.(25[0-5]|2[0-4]\\d|1\\d{2}|\\d{1,2})\\.(25[0-5]|2[0-4]\\d|1\\d{2}|\\d{1,2})"
        let regexText = NSPredicate(format: "SELF MATCHES %@", pattern)
        let result = regexText.evaluate(with: ip)
        return result
    }
    /*
    // MARK: - Navigation
    
    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destination.
        // Pass the selected object to the new view controller.
    }
    */

}

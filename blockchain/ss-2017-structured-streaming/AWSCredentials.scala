// Databricks notebook source
/**
 * WARNING: This class for storing credentials is insecure
 *          and should be used for demonstration purposes only
 */

import java.io.File
import java.io.PrintWriter

def writeToFile(p: String, s: String): Unit = {
    val f = new File(p)
    f.getParentFile().mkdirs();
    val pw = new PrintWriter(f)
    try pw.write(s) finally pw.close()
}

class AWSCredentials {
  private val _access = ""
  private val _secret = ""
  private var _endpoint = ""
  private var _region = ""

  def access = _access
  def secret = _secret
  def endpoint = _endpoint
  def region = _region
  
  def endpoint_(value: String): Unit = _endpoint = value
  def region_(value: String): Unit = _region = value
  
  def createSession(): Unit = {
    import java.io.File
    import java.io.PrintWriter
    
    val p = """
[default]
aws_access_key_id = """+this._access+"""
aws_secret_access_key = """+this._secret+"""
region = """+this.region
    
    val r = """
[default]
aws_access_key_id = """+this._access+"""
aws_secret_access_key = """+this._secret
    
    writeToFile("/root/.aws/config", p)
    writeToFile("/root/.aws/credentials", r)
  }
  
}
# -*- coding: utf-8 -*-

from datetime import datetime, timedelta
import json
import os
import sys

from flask import Flask, request, render_template, redirect
from flask.ext.sqlalchemy import SQLAlchemy

import config
import serial_interface

serial = serial_interface.SerialInterface('/dev/tty.usbmodem1421', 9600)

app = Flask(__name__)
app.config.from_object('config')

db = SQLAlchemy(app)

ILUMINACION = [
    'Oscuridad total',
    'Muy oscuro',
    'Oscuro',
    'Baja luz',
    'Buena luz',
    'Muy buena luz',
    'Luz de día'
]

def time_since(value, default="hace un instante"):
    now = datetime.utcnow()
    diff = now - value
    periods = (
        (diff.days / 365, "año", "años"),
        (diff.days / 30, "mes", "meses"),
        (diff.days / 7, "semana", "semanas"),
        (diff.days, "día", "días"),
        (diff.seconds / 3600, "hora", "horas"),
        (diff.seconds / 60, "minuto", "minutos"),
        (diff.seconds, "segundo", "segundos"),
    )
    for period, singular, plural in periods:
        if period:
            return "hace %d %s" % (period, singular if period == 1 else plural)
    return default

class Module(db.Model):

    __tablename__ = 'module'
    id = db.Column(db.Integer, primary_key=True)
    i2c_id = db.Column(db.Integer, unique=True)
    type = db.Column(db.String(50))
    name = db.Column(db.String(50), unique=True)
    value = db.Column(db.Integer)
    last_update = db.Column(db.DateTime)

    def as_dict(self):
        return {c.name: getattr(self, c.name) for c in self.__table__.columns}
    @property
    def desc(self):
        return '(%s) %s' % (self.type, self.name)
    @property
    def time_since(self):
        return time_since(self.last_update)
    @property
    def desc_value(self):
        if self.type == 'Aire':
            return 'Encendido' if self.value else 'Apagado'
        elif self.type == 'Lampara':
            return 'Encendida' if self.value else 'Apagada'
        elif self.type == 'Temperatura':
            return '%d C' % self.value
        elif self.type == 'Iluminacion':
            return '%d / 100' % self.value
        elif self.type == 'Persiana':
            return '%d%% cerrada' % self.value
        return '<sin datos>'
    def __repr__(self):
        return '<Module %r>' % (self.name)

class Rule(db.Model):

    __tablename__ = 'rule'
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(50), unique=True)
    time_from = db.Column(db.String(10))
    time_to = db.Column(db.String(10))
    sensor_module = db.Column(db.Integer)
    output_module = db.Column(db.Integer)
    less_than = db.Column(db.Boolean)
    value_threshold = db.Column(db.Integer)
    switch_to_value = db.Column(db.Integer)
    priority = db.Column(db.Integer)
    last_executed = db.Column(db.DateTime)
    sensor = None
    output = None

    def as_dict(self):
        return {c.name: getattr(self, c.name) for c in self.__table__.columns}
    @property
    def desc(self):
        return self.name
    @property
    def times(self):
        return '%s a %s' % (self.time_from, self.time_to)
    @property
    def sensors(self):
        self.sensor = Module.query.get(self.sensor_module)
        self.output = Module.query.get(self.output_module)
        return '%s controlando %s' % (
            self.sensor.desc, self.output.desc
        )
    def __repr__(self):
        return '<Rule %r>' % (self.name)

@app.route('/modules', methods=['GET'])
def modules():
    return json.dumps(Module.query.all())

@app.route('/module', methods=['POST'])
def add_module():
    values = { key: request.values[key] for key in request.values}
    try:
        values['last_update'] = datetime.fromtimestamp(float(values['last_update']))
    except:
        values['last_update'] = None
    mod = Module(**values)
    db.session.add(mod)
    db.session.commit()
    return ''

@app.route('/rules', methods=['GET'])
def rules():
    return json.dumps(Rule.query.all())

@app.route('/rule', methods=['POST'])
def add_rule():
    values = { key: request.values[key] for key in request.values }
    rule = Rule(**values)
    db.session.add(rule)
    db.session.commit()
    return json.dumps(rule.as_dict())

@app.route('/rule/<int:rule_id>', methods=['DELETE'])
def delete_rule(rule_id):
    db.session.delete(Rule.query.get(rule_id))
    db.session.commit()
    return redirect('/')

@app.route('/rule/delete/<int:rule_id>', methods=['POST'])
def delete_rule_alias(rule_id):
    db.session.delete(Rule.query.get(rule_id))
    db.session.commit()
    return redirect('/')

@app.route('/module/<int:module_id>', methods=['PUT'])
def send_data(module_id):
    with serial:
        serial.put(module_id, request.values['value'])
    return redirect('/')

@app.route('/index.html')
@app.route('/')
def homepage():
    return render_template('index.html',
        sensors=Module.query.all(),
        rules=Rule.query.all()
    );

@app.route('/module_cron')
def module_cron():
    current_datetime = datetime.now()
    for module in Module.query.all():
        with serial:
            output = serial.get(module.i2c_id)

        module.value = output
        module.last_update = current_datetime
        db.session.add(module)
        db.session.commit()
    return ''

@app.route('/rule_cron')
def rule_cron():
    current_datetime = datetime.now()
    current_time = current_datetime.strftime('%H:%M')
    for module in Module.query.all():
        for rule in Rule.query.filter_by(sensor_module=module.id).order_by('priority'):
            if current_time >= rule.time_from and current_time < rule.time_to:
                if ((rule.less_than and module.value <= rule.value_threshold)
                    or (not rule.less_than and module.value > rule.value_threshold)):
                    if True or not rule.last_executed or current_datetime - rule.last_executed > timedelta(minutes=10):
                        rule.last_executed = current_datetime

                        output_module = Module.query.get(rule.output_module)
                        with serial:
                            serial.put(output_module.i2c_id, rule.switch_to_value)

                        db.session.add(rule)
                        db.session.commit()
                        break
    return ''

@app.route('/module/put/<int:i2c>', methods=['POST'])
def send_to_module(i2c):
    with serial:
        serial.put(i2c, request.values['value'])
    return redirect('/')

@app.route('/add', methods=['GET'])
def add_rule_form():
    return render_template('add.html')

@app.route('/add', methods=['POST'])
def add_rule_process():
    values = { key: request.values[key] for key in request.values }
    values['less_than'] = values['less'] == 'Menor que'
    del values['less']
    rule = Rule(**values)
    db.session.add(rule)
    db.session.commit()
    return redirect('/')


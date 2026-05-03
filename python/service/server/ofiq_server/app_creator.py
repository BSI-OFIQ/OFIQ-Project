"""
creates a connexion app
"""
import os
import logging
import yaml
import connexion
from ofiq_server import ofiq_api_inst, encoder
from .ofiq_api import logger as ofiq_logger

def create():
    """creates a connexion app and initializes the OFIQ API server

    Returns
    -------
    FlaskApp
        a connextion/Flask app

    Raises
    ------
    ValueError
        if some required config parameters are missing an exeption is raised
    """

    ofiq_srv_cfg_file = os.environ.get(
        'OFIQ_SRV_CFG', 'ofiq_server_config.yaml')

    ofiq_srv_config: dict = {}
    with open(ofiq_srv_cfg_file, 'r', encoding='utf-8') as stream:
        ofiq_srv_config = yaml.safe_load(stream)

    # check if ofiq config section is present
    ofiq_config = ofiq_srv_config.get('ofiq')
    if not ofiq_config:
        raise ValueError("Missing required section 'ofiq' in config file: "
                         + ofiq_srv_cfg_file)

    # check if server config section is present
    srv_config = ofiq_srv_config.get('server')
    if not srv_config:
        raise ValueError("Missing required section 'server' in config file: "
                         + ofiq_srv_cfg_file)

    srv_port = srv_config.get('port', 8088)

    srv_loglevel: str = srv_config.get('loglevel')
    if srv_loglevel:
        logging_level = logging.getLevelName(srv_loglevel)
    else:
        logging_level = logging.WARNING
    connex_logger = logging.getLogger('connexion.app')
    connex_logger.setLevel(logging_level)

    log_formatter = logging.Formatter(
        '%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    # console output
    log_handler = logging.StreamHandler()
    log_handler.setFormatter(log_formatter)

    connex_logger.addHandler(log_handler)

    # initialize the server

    app = connexion.App(__name__, specification_dir='./swagger/')

    app.add_api('swagger.yaml',
                arguments={'title': 'OFIQ API'},
                jsonifier=encoder.JSONEncoder(),
                pythonic_params=True)

    app.app.logger.setLevel(logging_level)
    app.app.logger.debug(f'Server log level set to: {srv_loglevel}')

    # initialize the OFIQ module

    ofiq_data_dir = ofiq_config.get('data_dir')
    ofiq_cfg_file = ofiq_config.get('cfg_file')
    app.app.logger.debug(f'Initializing OFIQ using config file: {ofiq_cfg_file} in directory: {ofiq_data_dir}')

    ofiq_logger.setLevel(logging_level)
    ofiq_logger.addHandler(log_handler)

    init_res, err_str = ofiq_api_inst.initialize(ofiq_data_dir, ofiq_cfg_file)

    if not init_res:
        raise ValueError(f"OFIQ initialization failed: {err_str}")

    app.app.logger.debug('OFIQ initialized successfully')

    return app, srv_port

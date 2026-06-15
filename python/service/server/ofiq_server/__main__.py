#!/usr/bin/env python3

from ofiq_server import app_creator

def main():
    """creates and runs the connexion/Flask app
    """
    app, srv_port = app_creator.create()
    app.run(port=srv_port)

    # app = connexion.App(__name__, specification_dir='./swagger/')
    # app.app.json_encoder = encoder.JSONEncoder
    # app.add_api('swagger.yaml', arguments={'title': 'OFIQ API'}, pythonic_params=True)
    # app.run(port=8080)


if __name__ == '__main__':
    main()

import { Server } from "miragejs";

export default function mockApi() {
  if (process.env.NODE_ENV === 'development') {
    new Server({
      routes() {
        this.namespace = "api";

        this.get("/sprinkler", () => {
          return {
            sprinklers: [
              {"id":0,"start":"00:00","duration":2,"days":[
                {"0":true},{"1":false},{"2":false},{"3":false},{"4":false},{"5":false},{"6":false},{"7":false}
              ]},
              {"id":1,"start":"01:00","duration":4,"days":[
                {"0":false},{"1":true},{"2":false},{"3":false},{"4":false},{"5":false},{"6":false},{"7":false}
              ]},
              {"id":2,"start":"02:00","duration":6,"days":[
                {"0":false},{"1":false},{"2":true},{"3":false},{"4":false},{"5":false},{"6":false},{"7":false}
              ]},
              {"id":3,"start":"03:00","duration":8,"days":[
                {"0":false},{"1":false},{"2":false},{"3":true},{"4":false},{"5":false},{"6":false},{"7":false}
              ]},
              {"id":4,"start":"04:00","duration":10,"days":[
                {"0":false},{"1":false},{"2":false},{"3":false},{"4":true},{"5":false},{"6":false},{"7":false}
              ]},
              {"id":5,"start":"05:00","duration":12,"days":[
                {"0":false},{"1":false},{"2":false},{"3":false},{"4":false},{"5":true},{"6":false},{"7":false}
              ]},
              {"id":6,"start":"06:00","duration":14,"days":[
                {"0":false},{"1":false},{"2":false},{"3":false},{"4":false},{"5":false},{"6":true},{"7":false}
              ]},
              {"id":7,"start":"07:00","duration":16,"days":[
                {"0":true},{"1":false},{"2":false},{"3":false},{"4":false},{"5":false},{"6":false},{"7":false}
              ]}
            ]
          };
        });
      }
    });
  }
}
